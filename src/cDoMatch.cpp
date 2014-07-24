/*
*	File : cDoMatch.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/08/2014
*	Version : 1.0
*	
*	Description : Program to make match in parallel
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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <mpi.h>

#include "directory.h"
#include "util.h"
#include "dosift.h"
#include "domatch.h"

using namespace std;
using namespace cv;


/* 
*	Function : boss
*	Description : code for the boss
*	
*	numcore : number of cores
*	dir : directory information
*/
int* boss(int numcore, const util::Directory &dir)
{
	int* dis = createDist4Match(dir.getNBImages(), numcore);

	return dis;
}


/* 
*	Function : serializeVector
*	Description : to serialize a vector
*	
*	sender : vector to serialize
*	img1 : images 1 index
*	img2 : images 2 index
*/
float* serializeVector(vector<DMatch> tab, int img1, int img2)
{
	float* serialTab;

	int s = tab.size() * 2 + 4;

	serialTab = (float*) malloc(s * sizeof(float));

	serialTab[0] = (float) s;
	serialTab[1] = (float) tab.size();
	serialTab[2] = (float) img1;
	serialTab[3] = (float) img2;

	for(int i = 0; i < tab.size(); i++)
	{
		serialTab[4+i*2] = (float) tab[i].queryIdx;
		serialTab[5+i*2] = (float) tab[i].trainIdx;
	}

	return serialTab;
}


/* 
*	Function : endComm
*	Description : implementation to end a conversation
*	
*	sender : sender (worker) ID
*/
void endComm(int sender)
{
	float endSignal = -1.0;
	MPI_Send(&endSignal, 1, MPI_FLOAT, sender, 0, MPI_COMM_WORLD);
} 


/* 
*	Function : worker
*	Description : code for the workers
*	
*	dir : directory information
*	recv : relative information about distribution
*/
void worker(const util::Directory &dir, int* recv)
{
	int aim = recv[0];
	int end = recv[1];

	int numimages, netID, seek = 0, compute = 0, stop = 0;

	numimages = dir.getNBImages();

	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	vector<string> list;

	listDir(dir, list);

	int size = 200000000*sizeof(float);

	float* buf = (float*) malloc(size);

	struct SFeatures keys1, keys2;
	struct Matches container;

	float* serialMatch;		 

	MPI_Buffer_attach(buf, size);

	for(int i = 0; !stop; i++)
	{
		if(compute)
		{
			readSiftFile(list[i], keys1);
		}

		for (int j = 0; j < i; j++)
		{

			if(seek == aim){readSiftFile(list[i], keys1);compute=1;}

			if(compute)
			{
				readSiftFile(list[j], keys2);

				doMatch(keys1, keys2, container);

				serialMatch = serializeVector(container.matches, j, i);

				//cout << "[CORE " << netID << "]: " << container.NM << " match(es) found between " << dir.getImage(j) << " and " << dir.getImage(i) << endl;

				container.reset();

				MPI_Bsend(serialMatch, serialMatch[0], MPI_FLOAT, SECRETARY, 1, MPI_COMM_WORLD);

				free(serialMatch);
			}

			seek++;
			if(seek == end){compute=0;stop=1;}
		}
	}

	MPI_Buffer_detach(buf, &size);
	endComm(SECRETARY);
}


/* 
*	Function : writeSerialMatch
*	Description : code to write in the file from a serial matches structure
*	
*	f : file descriptor
*	serialMatches : serial structure
*/
void writeSerialMatch(FILE* f, float* serialMatches)
{
	fprintf(f, "%d %d\n%d\n", (int) serialMatches[2], (int) serialMatches[3], (int) serialMatches[1]);

	int end = (int) serialMatches[1] * 2 + 4;

	for(int i = 4; i < end; i+=2)
	{
		fprintf(f, "%d %d\n", (int) serialMatches[i], (int) serialMatches[i+1]);
	}
}

/* 
*	Function : recvFromWorker
*	Description : recv implementation for the secretary
*/
float* recvFromWorker()
{
	MPI_Status status;
	float* serialMatch; 
	float garbage;
	int sender, s;

	MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	int tag = status.MPI_TAG;
	sender = status.MPI_SOURCE;

	if(tag > 0)
	{
		MPI_Get_count(&status, MPI_FLOAT, &s);

		serialMatch = (float*) malloc(s * sizeof(float));
		MPI_Recv(serialMatch, s, MPI_FLOAT, sender, 1, MPI_COMM_WORLD, &status);
	}
	else
	{
		MPI_Recv(&garbage, 1, MPI_FLOAT, sender, tag, MPI_COMM_WORLD, &status);
		serialMatch = NULL;
	}

	return serialMatch; 
}

/* 
*	Function : secretary
*	Description : code for the secretary
*	
*	path : directory path
*	numcore : number of cores
*/
void secretary(const string &path, int numcore, int n)
{
	vector<float*> v_serialMatch;
	float* serialMatch;

	int end = 1, i = 0, n = ;

	string file(path);
	file.append(MATCHFILE);

	FILE* f = fopen(file.c_str(), "w");

	while(end < numcore)
	{
		serialMatch = recvFromWorker();
		if (serialMatch)
		{
			v_serialMatch.push_back(serialMatch);
		}
		else
		{
			end++;
		}
		i++;
		showProgress(i, n, 75, 1)
	}

	showProgress(n, n, 75, 0)

	cout << "--> Écriture dans le fichier..." << endl;

	end = v_serialMatch.size();

	for (int i = 0; i < end; i++)
	{
		writeSerialMatch(f, v_serialMatch[i]);
		free(v_serialMatch[i]);
	}

	fclose(f);
}


int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int* dist;
	int recv[2];

	double the_time;

	int netSize;
	int netID;

	MPI_Comm_size(MPI_COMM_WORLD, &netSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	util::Directory dir(argv[1]);
	struct Matches container;

	if(netSize < 2)
	{
		if(netID == 0)
		{
			printf("[ERROR] At most 2 cores are needed\n");
		}
		exit(1);
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
		secretary(dir.getPath(), netSize, n);
	}
	else
	{
		worker(dir, recv);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0)
	{
		printf("The program takes approximately %f second(s)\n", MPI_Wtime() - the_time);
	}

	MPI_Finalize();

	return 0;
}












