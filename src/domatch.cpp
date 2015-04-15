/*
*	File : domatch.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, July 8th
*	Version : 1.0
*
*	Description : Program to match sift points in parallel
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

#include "directory.h"
#include "util.h"
#include "libmatch.h"

using namespace std;
using namespace cv;


int main(int argc, char* argv[])
{
	//MPI initialization
	MPI_Init(&argc, &argv);

	//For execution time calculation
	double the_time;

	//Parse the facultative arguments
	struct mArgs args;

	mParseArgs(argc, argv, &args);

	//Create a object to store the working directory information
	util::Directory dir(args.workingDir.c_str());
	struct SFeatures container;

	//Set verbose mode and geometry mode
	int verbose = args.verbose;
	int geo = args.geometry;

	//Number of cores and the ID of the core
	int netSize;
	int netID;

	//request the number of cores and the ID of the core
	MPI_Comm_size(MPI_COMM_WORLD, &netSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	if(netSize < 2) {
		printf("[ERROR] At most 2 cores are needed\n");
		exit(1);
	}

	//Set the starting and ending index
	int start, end;
	distribution(netID, netSize, dir, DIST4MATCHES, &start, &end);

	//Print the distribution information, MPI_Send/MPI_Recv are more portable
	//Than MPI_Barrier for printing. Hence it can be always fully synchronised
	if(netID == 0 && verbose) {
		MPI_Status status;
		int buffer[3];

		printf(" --> Here's the distribution :\n");

		printf("	Core 0 will compute images %5d to %5d\n", start, end);

		for(int i = 1; i < netSize; i++) {
			MPI_Recv(&buffer, 3, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			printf("	Core %d will compute pairs %5d to %5d\n", buffer[0], buffer[1], buffer[2]);
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

	if(netID == 0)
		the_time = MPI_Wtime();

	MPI_Barrier(MPI_COMM_WORLD);

	worker(dir, start, end, args, netID, netSize);

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0 && verbose) {
		double time_r = MPI_Wtime() - the_time;
		int h = int(time_r/3600);
		int m = int(time_r/60) - h*60;
		double s = time_r - h*3600 - m*60;
		printf(" --> The sift search takes %dh %dm %0.3fs\n", h, m, s);
	}

	MPI_Finalize();

	return 0;
}
