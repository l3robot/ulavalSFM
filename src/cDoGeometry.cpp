/*
*	File : cDoGeometry.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/08/2014
*	Version : 1.0
*	
*	Description : Program to calculate geometric constraints in parallel
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <mpi.h>

#include "directory.h"
#include "util.h"
#include "domatch.h"
#include "dogeometry.h"

using namespace std;
using namespace util;


/* 
*	Function : boss
*	Description : code for the boss
*	
*	numcore : number of cores
*	dir : directory information
*	container : constraints information
*/
int* boss(int numcore, const util::Directory &dir, struct Constraints &container)
{
	int* dis = createDist4Geometry(container.NP, numcore);

	return dis;
}



/* 
*	Function : pruneDoubleMatch
*	Description : Eliminate the double matches, keep the first only. If a pair have less than 20 matches, the pair is eliminated.
*	
*	box : container for the match information
*/
void pruneDoubleMatch(struct Matchespp &box, int id)
{
	struct Matchespp new_box(box.idx);
	
	pruneDoubleMatch(new_box, box);

	//cout << "[CORE " << id << "] Pruned " << box.NM - new_box.NM << " out of " << box.NM << endl;

	if (new_box.NM >= 20) box.assign(new_box);
	else box.reset();
}

/* 
*	Function : fMatrixFilter
*	Description : Estimate FMatrix with RANSAC and keep inliers only
*	
*	features : information on features
*	box : information on the match
*	treshold : treshold for the RANSAC algorithm
*/
void fMatrixFilter(const vector<struct SFeatures> &features, struct Matchespp &box, int id)
{
	int NI;

	NI = fMatrixFilter(features[box.idx[0]].keys, features[box.idx[1]].keys, box.matches);

	//cout << "[CORE " << id << "] Filtered " << box.NM - NI << " out of " << box.NM << endl;

	if (NI >= 16) box.NM = NI;
	else box.reset();
}



/* 
*	Function : transformInfo
*	Description : Find Transform, compute inliers and its ratio
*	
*	features : information on features
*	box : information on the match
*	treshold : treshold for the RANSAC algorithm
*/
void transformInfo(const vector<struct SFeatures> &features, struct Matchespp &box, int id)
{
	transformInfo(features[box.idx[0]].keys, features[box.idx[1]].keys, box);

	//cout << "[CORE " << id << "] " << box.NI << " inliers out of " << box.NM << endl;

	if (box.NI < 10) box.reset(false);
}


float* serializeMatchespp(const struct Matchespp &box)
{
	int size = 15 + box.NM * 2;

	float* serialMatchespp = (float*) malloc(size * sizeof(float));

	serialMatchespp[0] = (float) size;
	serialMatchespp[1] = (float) box.idx[0];
	serialMatchespp[2] = (float) box.idx[1];
	serialMatchespp[3] = (float) box.NM;

	int end = box.NM * 2 + 4;
	int num = 0;

	for (int i = 4; i < end; i+=2)
	{
		serialMatchespp[i] = (float) box.matches[num].queryIdx;
		serialMatchespp[i+1] = (float) box.matches[num].trainIdx;
		num++;
	}

	serialMatchespp[end] = (float) box.NI;

	num = 0;

	const double* pter = box.H.ptr<double>();
	
	for (int i = end + 1; i < end + 10; i++)
	{
		serialMatchespp[i] = (float) pter[num];
		num++;
	}

	serialMatchespp[end+10] = box.ratio;

	return serialMatchespp;
}

void destroyMatchespp(float* serialMatchespp)
{
	free(serialMatchespp);
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
void worker(const util::Directory &dir, int* recv, struct Constraints &container)
{
	int start = recv[0];
	int end = recv[1];

	int netID;

	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	vector<string> list;

	listDir(dir, list);

	int size = 200000000*sizeof(float);

	float* buf = (float*) malloc(size);

	float* serialMatch;		 

	MPI_Buffer_attach(buf, size);

	for(int i = start; i < end; i++)
	{
		pruneDoubleMatch(container.matches[i], netID);
		if (container.matches[i].NM > 0) fMatrixFilter(container.features, container.matches[i], netID);
		if (container.matches[i].NM > 0) 
		{
			transformInfo(container.features, container.matches[i], netID);

			serialMatch = serializeMatchespp(container.matches[i]);

			cout << "[CORE " << netID << "]: " << container.matches[i].NI << " inlier(s) found between " << container.matches[i].idx[0] << " and " << container.matches[i].idx[1] << endl;

			MPI_Bsend(serialMatch, serialMatch[0], MPI_FLOAT, SECRETARY, 1, MPI_COMM_WORLD);

			destroyMatchespp(serialMatch);
		}
	}

	//MPI_Buffer_detach(buf, &size);
	endComm(SECRETARY);
}


/* 
*	Function : recvFromWorker
*	Description : recv implementation for the secretary
*/
float* recvFromWorker()
{
	MPI_Status status;
	float* serialMatchespp;
	int sender, s;

	MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	int tag = status.MPI_TAG;

	if(tag > 0)
	{
		sender = status.MPI_SOURCE;
		MPI_Get_count(&status, MPI_FLOAT, &s);

		serialMatchespp = (float*) malloc(s * sizeof(float));
		MPI_Recv(serialMatchespp, s, MPI_FLOAT, sender, 1, MPI_COMM_WORLD, &status);
	}
	else
	{
		serialMatchespp = NULL;
	}

	return serialMatchespp; 
}

/* 
*	Function : writeSerialMatchespp
*	Description : code to write in the file from a serial matchespp structure
*	
*	path : directory path
*	serialMatchespp : serial structure
*/
void writeSerialMatchespp(const string &path, const vector<float*> &container)
{
	string file1(path);
	string file2(path);

	file1.append("matches.init.txt");
	file2.append("ulavalSFM.txt");

	FILE *f1 = fopen(file1.c_str(), "wb");
	FILE *f2 = fopen(file2.c_str(), "wb");

	int NP = container.size();
	int NT = 0;

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

			fprintf(f2, "%d %d\n", (int) container[i][1], (int) container[i][2]);

	        fprintf(f2, "%d\n", (int) container[i][num]);
	        fprintf(f2, "%f\n", container[i][num + 10]);

	        fprintf(f2, "%f %f %f %f %f %f %f %f %f\n", container[i][num + 1], container[i][num + 2], 
	        	container[i][num + 3], container[i][num + 4], container[i][num + 5], container[i][num + 6], 
	        	container[i][num + 7], container[i][num + 8], container[i][num + 9]);
		}
	}

	fseek(f2, 0, SEEK_SET);
	fprintf(f2, "%d\n", NT);

	fclose(f1);
	fclose(f2);
}

/* 
*	Function : secretary
*	Description : code for the secretary
*
*	path : directory path
*	numcore : number of cores
*/
void secretary(const string &path, int numcore)
{
	vector<float*> v_serialMatchespp;
	float* serialMatchespp;

	int end = 0;

	serialMatchespp = recvFromWorker();

	while(end < numcore)
	{
		if (serialMatchespp)
		{
			v_serialMatchespp.push_back(serialMatchespp);
		}
		else
		{
			end++;
		}
		serialMatchespp = recvFromWorker();
	}

	cout << "--> Écriture dans le fichier..." << endl;

	end = v_serialMatchespp.size();

	writeSerialMatchespp(path, v_serialMatchespp);

	//freeing memory
	for (int i = 0; i < end; i++)
	{
		free(serialMatchespp);
	}
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
	struct Constraints container;

	readKeys(dir, container);
	readMatchFile(dir.getPath(), container);

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
		dist = boss(netSize, dir, container);
	}

	MPI_Scatter(dist, 2, MPI_INT, recv, 2, MPI_INT, 0, MPI_COMM_WORLD);

	if(netID == 0)
	{
		deleteDist(dist);
		secretary(dir.getPath(), netSize);
	}
	else
	{
		worker(dir, recv, container);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(netID == 0)
	{
		printf("The program takes approximately %f second(s)\n", MPI_Wtime() - the_time);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();

	return 0;
}














