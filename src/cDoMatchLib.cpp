/*
*	File : cDoMatchLib.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/31/2014
*	Version : 1.0
*	
*	Description : Functions to make match in parallel
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
#include "dogeometry.h"
#include "cDoMatchLib.h"

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
float* serializeContainer(const struct Matchespp &container)
{
	float* serialTab;

	int s = 15 + container.NM * 2;

	serialTab = (float*) malloc(s * sizeof(float));

	serialTab[0] = (float) s;
	serialTab[1] = (float) container.idx[0];
	serialTab[2] = (float) container.idx[1];
	serialTab[3] = (float) container.NM;

	for(int i = 0; i < container.NM * 2; i+=2)
	{
		int idx = i + 4;
		int idx2 = i/2;
		serialTab[idx] = (float) container.matches[idx2].queryIdx;
		serialTab[idx+1] = (float) container.matches[idx2].trainIdx;
	}

	int seek = 4 + container.NM * 2;

	serialTab[seek] = (float) container.NI;

	const double* M = container.H.ptr<double>();

	serialTab[seek+1] = (float) M[0];
	serialTab[seek+2] = (float) M[1];
	serialTab[seek+3] = (float) M[2];
	serialTab[seek+4] = (float) M[3];
	serialTab[seek+5] = (float) M[4];
	serialTab[seek+6] = (float) M[5];
	serialTab[seek+7] = (float) M[6];
	serialTab[seek+8] = (float) M[7];
	serialTab[seek+9] = (float) M[8];

	serialTab[seek+10] = container.ratio;

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

	struct SFeatures keys1, keys2;

	float* serialMatch;		 

	for(int i = 0; !stop; i++)
	{
		if(compute)
		{
			readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), list[i], keys1);
		}

		for (int j = 0; j < i; j++)
		{

			if(seek == aim){readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), list[i], keys1);compute=1;}

			if(compute)
			{
				struct Matchespp container(j, i);

				readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), list[j], keys2);

				doMatch(keys1, keys2, container);

				serialMatch = serializeContainer(container);

				//cout << "[CORE " << netID << "]: " << container.NM << " match(es) found between " << dir.getImage(j) << " and " << dir.getImage(i) << endl;

				container.reset();

				MPI_Send(serialMatch, serialMatch[0], MPI_FLOAT, SECRETARY, 1, MPI_COMM_WORLD);

				free(serialMatch);
			}

			seek++;
			if(seek == end){compute=0;stop=1;}
		}
	}

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
*	Function : writeSerialMatchespp
*	Description : code to write in the file from a serial matchespp structure
*	
*	path : directory path
*	serialMatchespp : serial structure
*/
void writeSerialMatchespp(const string &path, const vector<float*> &container, int bar)
{
	string file1(path);
	string file2(path);

	file1.append("matches.init.txt");
	file2.append("ulavalSFM.txt");

	FILE *f1 = fopen(file1.c_str(), "wb");
	FILE *f2 = fopen(file2.c_str(), "wb");

	int NP = container.size();
	int NT = 0;

	fprintf(f2, "\n");

	for (int i = 0; i < NP; i++)
	{
		int NM = (int) container[i][3];

		fprintf(f1, "%d %d\n", (int) container[i][1], (int) container[i][2]);
		fprintf(f1, "%d\n", NM);

		int num = 4;

		for(int j = 0; j < NM; j++)
		{
			fprintf(f1, "%d %d\n", (int) container[i][num], (int) container[i][num + 1]);
			num += 2;
		}

		if(container[i][num] > 0)
		{
			NT++;
			//BUG À RÉGLER ICI Premiers ne sont pas affichés
			fprintf(f2, "%d %d\n", (int) container[i][1], (int) container[i][2]);

	        fprintf(f2, "%d\n", (int) container[i][num]);
	        fprintf(f2, "%f\n", container[i][num + 10]);

	        fprintf(f2, "%f %f %f %f %f %f %f %f %f\n", container[i][num + 1], container[i][num + 2], 
	        	container[i][num + 3], container[i][num + 4], container[i][num + 5], container[i][num + 6], 
	        	container[i][num + 7], container[i][num + 8], container[i][num + 9]);
		}

		if (bar) showProgress(i, NP, 75, 1);
	}

	if (bar) showProgress(NP, NP, 75, 0);

	fseek(f2, 0, SEEK_SET);
	fprintf(f2, "%d\n", NT);

	fclose(f1);
	fclose(f2);
}

/* 
*	Function : recvFromWorker
*	Description : recv implementation for the secretary
*/
float* recvFromWorker(vector<int> &list)
{
	MPI_Status status;
	float* serialMatch; 
	float garbage;
	int sender, s;

	MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	int tag = status.MPI_TAG;
	sender = status.MPI_SOURCE;

	list.push_back(sender);

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
void secretary(const string &path, int numcore, int n, int bar)
{
	vector<float*> v_serialMatch;
	vector<int> list;
	float* serialMatch;

	int end = 1, i = 0;

	string file(path);
	file.append(MATCHFILE);

	printf("--> Matching : \n");

	n = n + numcore - 1;

	while(end < numcore)
	{
		serialMatch = recvFromWorker(list);
		if (serialMatch)
			v_serialMatch.push_back(serialMatch);
		else
			end++;
		i++;
		if (bar) showProgress(i, n, 75, 1);
	}

	if (bar) showProgress(n, n, 75, 0);

	cout << "--> Writing file : " << endl;

	writeSerialMatchespp(path, v_serialMatch, bar);

	cout << endl;
}