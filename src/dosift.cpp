/*
*	File : cDoSift.cpp
*	Author : �mile Robitaille @ LERobot
*	Creation date : 2014, June 30th
*	Version : 1.0
*
*	Description : Program to find sift point on a image dataset in parallel
*/

#include <stdio.h>
#include <stdlib.h>
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

	export char verbose = 0;
	export char *siftPath = NULL;

	//Check the number of arguments
	if (argc < 2)
		sUsage(argv[0]);

	//Parse the facultative arguments
	sParseArgs(argc, argv);

	//Create the strings for working directory and sift file storage directory
	string img(argv[1]);
	string key(siftPath);

	//Create a object to store the working directory information
	util::Directory dir(argv[1]);
	struct SFeatures container;

	//Number of cores and the ID of the core
	int netSize;
	int netID;

	//request the numbre of cores and the ID of the core
	MPI_Comm_size(MPI_COMM_WORLD, &netSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	//Set the starting and ending index
	int start, end;
	distribution(netID, netSize, dir, DIST4SIFT, &start, &end);

	//Waiting all the cores
	MPI_Barrier(MPI_COMM_WORLD);

	//Brief reminder of what the program will do
	if (netID == 0) {
		double the_time = MPI_Wtime();
		printf("--> Sift searching begins on %d core(s) :\n", netSize);
	}

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
	if(netID == 0) {
		printf("The sift search takes approximately %0.3f second(s)\n", MPI_Wtime() - the_time);
	}

	//Terminate MPI
	MPI_Finalize();

	return 0;
}
