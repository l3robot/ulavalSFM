/*
*	File : cDoAll.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/30/2014
*	Version : 1.0
*	
*	Description : Program to make all the algorithms in parallel
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
#include "dosift.h"
#include "domatch.h"
#include "dogeometry.h"
#include "cDoMatch.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int* dist;
	int recv[2];
	int bar = 1;

	//quick parsing of bar printing or not
	if(argc > 2)
	{
		if(argv[2][0] == '0')
			bar = 0;
	}

	double the_time;

	int netSize;
	int netID;

	MPI_Comm_size(MPI_COMM_WORLD, &netSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	util::Directory dir(argv[1]);
	struct SFeatures container;
	string file(argv[1]);

	if(netSize < 2)
	{
		if(netID == 0)
		{
			printf("[ERROR] At most 2 cores are needed\n");
		}
		exit(1);
	}

	if(netID == 0)
	{
		dist = createDist(dir, netSize);
	}

	MPI_Scatter(dist, 2, MPI_INT, recv, 2, MPI_INT, 0, MPI_COMM_WORLD);

	// SIFT

	if(netID == 0)
	{
		deleteDist(dist);
		the_time = MPI_Wtime();
	}

	MPI_Barrier(MPI_COMM_WORLD);

	int start = recv[0];
	int end = recv[1];

	if (netID == 0)
		printf("--> Sift searching begins on %d core(s) :\n", netSize);

	for(int i = start; i < end; i++)
	{
		file.append(dir.getImage(i));

		doSift(file, container);

		//cout << "[CORE " << netID << "]: " << container.keys.size() << " sift point(s) found in " << dir.getImage(i) << endl;

		while (file[file.size() - 1] != '.')
		{
			file.pop_back();
		}

		file.append("key");

		writeSiftFile(file, container);

		while (file[file.size() - 1] != '/')
		{
			file.pop_back();
		}

		if (netID == 0 && bar)
			showProgress(i, end, 75, 1);
	} 

	if (netID == 0 && bar)
		showProgress(end, end, 75, 0);

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0)
	{
		printf("The sift search takes approximately %f second(s)\n", MPI_Wtime() - the_time);
		the_time = MPI_Wtime();
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0)
	{
		the_time = MPI_Wtime();
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0)
	{
		dist = boss(netSize, dir);
	}

	MPI_Scatter(dist, 2, MPI_INT, recv, 2, MPI_INT, 0, MPI_COMM_WORLD);

	if(netID == 0)
	{
		int n = dir.getNBImages() * (dir.getNBImages() - 1) / 2;
		deleteDist(dist);
		secretary(dir.getPath(), netSize, n, bar);
	}
	else
	{
		worker(dir, recv);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0)
	{
		printf("The matching phase takes approximately %f second(s)\n", MPI_Wtime() - the_time);
		string path(argv[1]);
		the_time = MPI_Wtime();
		Bundler(path);
		printf("The bundlerSFM phase takes approximately %f second(s)\n", MPI_Wtime() - the_time);
	}

	MPI_Finalize();

	return 0;
}












