/*
*	File : cDoSift.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 30th
*	Version : 1.0
*
*	Description : Program to find sift point on a image dataset in parallel
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <mpi.h>

#include "directory.h"
#include "util.h"
#include "libsift.h"

using namespace std;
using namespace util;

int main(int argc, char* argv[])
{
	//MPI initialization
	MPI_Init(&argc, &argv);

	//For execution time calculation
	double the_time;

	//Parse the facultative arguments
	struct sArgs args;

	sParseArgs(argc, argv, &args);

	//Create a object to store the working directory information
	util::Directory dir(args.workingDir.c_str());
	struct SFeatures container;

	//Create the strings for working directory and sift file storage directory
	string img(args.workingDir);
	string key(args.siftDir);

	//Set verbose mode
	int verbose = args.verbose;

	//Number of cores and the ID of the core
	int netSize;
	int netID;

	//request the numbre of cores and the ID of the core
	MPI_Comm_size(MPI_COMM_WORLD, &netSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	//Set the starting and ending index
	int start, end;
	distribution(netID, netSize, dir, DIST4SIFT, &start, &end);

	//Print the distribution information, MPI_Send/MPI_Recv are more portable
	//Than MPI_Barrier for printing. Hence it can be always fully synchronised
	if(netID == 0 && verbose) {
		MPI_Status status;
		int buffer[3];

		printf(" --> Here's the distribution :\n");

		printf("	Core 0 will compute images %5d to %5d\n", start, end);

		for(int i = 1; i < netSize; i++) {
			MPI_Recv(&buffer, 3, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			printf("	Core %d will compute images %5d to %5d\n", buffer[0], buffer[1], buffer[2]);
		}
	}
	else if(verbose) {
		int buffer[3];
		buffer[0] = netID;
		buffer[1] = start;
		buffer[2] = end;
		MPI_Send(&buffer, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//Brief reminder of what the program will do
	if (netID == 0 && verbose) {
		the_time = MPI_Wtime();
		printf(" --> Sift searching begins on %d core(s) :\n", netSize);
	}

	//Verbose mode
	if (netID == 0 && verbose)
		showProgress(0, end, 75, 1);

	//Main loop
	for(int i = start; i < end; i++)
	{
		img.append(dir.getImage(i));
		key.append(dir.getImage(i));

		doSift(img, container);

		while (key[key.size() - 1] != '.')
			key.pop_back();

		key.append("key");

		writeSiftFile(key, container);

		while (img[img.size() - 1] != '/')
			img.pop_back();

		while (key[key.size() - 1] != '/')
			key.pop_back();

		//Verbose mode
		if (netID == 0 && verbose)
			showProgress(i, end, 75, 1);
	}

	//Verbose mode
	if (netID == 0 && verbose)
		showProgress(end, end, 75, 0);

	//Waiting all the cores
	MPI_Barrier(MPI_COMM_WORLD);

	//Print the time it needs to compute all the images
	if(netID == 0 && verbose) {
		double time_r = MPI_Wtime() - the_time;
		int h = int(time_r/3600);
		int m = int(time_r/60) - h*60;
		double s = time_r - h*3600 - m*60;
		printf(" --> The sift search takes %dh %dm %0.3fs\n", h, m, s);
	}

	//Terminate MPI
	MPI_Finalize();

	return 0;
}
