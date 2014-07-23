/*
*	File : dogeometry.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/08/2014
*	Version : 1.0
*	
*	Description : Functions relative to geometry
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "domatch.h"
#include "dogeometry.h"
#include "directory.h"

using namespace std;
using namespace cv;



/* 
*	Function : readMatchFile
*	Description : Read the match file, adjust the coordinates and fill a SFeatures object
*	
*	file : path of the .key file
*	container : container for sift keypoints and their descriptor
*/
void readAndAdjustSiftFile(const string &path, const string &img, const string &file, struct SFeatures &container)
{
	Mat buffer;

	container.keys.clear();

	string true_img = path;
	true_img.append(img);

	buffer = imread(true_img.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

	float height = (float) buffer.rows;
	float width = (float) buffer.cols;

	FILE* f = fopen(file.c_str(), "r");

	if(!f)
	{
		cout << "[ERROR] The file " << file << " does not exist" << endl;
		cout << "[ERROR] Maybe you haven't found sift point yet" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	float garbage, *pter;

	fscanf(f, "%d %f", &container.NF, &garbage);

	Mat descriptor(container.NF, 128, CV_32F);
	KeyPoint key;

	for(int i = 0; i < container.NF; i++)
	{
		fscanf(f, "%f %f %f %f", &key.pt.y, &key.pt.x, &garbage, &garbage);

		//reverse y axis
		key.pt.y = height - key.pt.y - 1.0;

		//put the image center at (0, 0)
		key.pt.x -= 0.5 * (width - 1);
        key.pt.y -= 0.5 * (height - 1);

		container.keys.push_back(key);

		pter = descriptor.ptr<float>(i);

		for(int j = 0; j < 128; j++)
		{
			fscanf(f, "%f", &pter[j]);
		} 
	}

	container.des = descriptor;

	fclose(f);	
}

/* 
*	Function : readKeys
*	Description : Load the keys in a Constraints object
*	
*	dir : directory information
*	container : container for constraints information
*/
void readKeys(const util::Directory &dir, struct Constraints &container)
{
	int numimage = dir.getNBImages();
	struct SFeatures features;
	vector<string> list;

	listDir(dir, list);

	for(int i = 0; i < numimage; i++)
	{
		readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), list[i], features);
		container.features.push_back(features);
	}
}


/* 
*	Function : readMatchFile
*	Description : Read the match file and fill the constraints object
*	
*	container : container for constraints information
*/
void readMatchFile(const string &path, struct Constraints &container)
{
	struct Matchespp box;
	char buffer[256];

	string file(path);

	file.append(MATCHFILE);

	FILE* f = fopen(file.c_str(), "r");

	if(!f)
	{
		cout << "[ERROR] The file \"" << file << "\" does not exist" << endl;
		cout << "[ERROR] Maybe you haven't match sift point yet" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	int NM = 0;

	while(fgets(buffer, 256, f))
	{

		sscanf(buffer, "%d %d", &box.idx[0], &box.idx[1]);
		fscanf(f, "%d\n", &NM);
		box.NM = NM; 
		
		for(int i = 0; i < NM; i++)
		{
			int query;
			int train;
			fscanf(f, "%d %d\n", &query, &train);
			box.matches.push_back(DMatch(query, train, 0.0));
		}

		container.matches.push_back(box);
		container.NP++;
		box.matches.clear();
	}

	cout << endl;

	fclose(f);
}


/* 
*	2 Functions : pruneDoubleMatch
*	Description : Eliminate the double matches, keep the first only. If a pair have less than 20 matches, the pair is eliminated.
*	
*	container : container for constraints information
*/
void pruneDoubleMatch(struct Constraints &container)
{
	struct Constraints new_container;

	for(int i = 0; i < container.NP; i++)
	{
		if (container.matches[i].NM < 20)
			cout << "DOUBLE : [ " << container.matches[i].idx[0] << ", " << container.matches[i].idx[1] << " ] : " <<  "Too few matches, considered like if there were no matches" << endl;
		else
		{
			struct Matchespp new_box(container.matches[i].idx);
			pruneDoubleMatch(new_box, container.matches[i]);
			cout << "DOUBLE : [ " << container.matches[i].idx[0] << ", " << container.matches[i].idx[1] << " ] : " <<  new_box.NM << " kept out of " << container.matches[i].NM << endl;
			if (new_box.NM < 20)
				cout << "DOUBLE : [ " << container.matches[i].idx[0] << ", " << container.matches[i].idx[1] << " ] : " <<  "Too few matches, considered like if there were no matches" << endl;
			else {new_container.matches.push_back(new_box); new_container.NP++;}
		}
	}

	container.assignMatches(new_container);
}

void pruneDoubleMatch(struct Matchespp &new_box, const struct Matchespp &box)
{
	int NM = box.NM;
	unordered_set<int> memory;
	for (int i = 0; i < NM; i++)
	{
		if(memory.find(box.matches[i].trainIdx) == memory.end())
		{
			new_box.matches.push_back(box.matches[i]);
			new_box.NM++;
			memory.insert(box.matches[i].trainIdx);
		}
	}
}

/* 
*	2 Functions : fMatrixFilter
*	Description : Estimate FMatrix with RANSAC and keep inliers only
*	
*	file : path of the .sift file
*	container : container for sift keypoints and their descriptor
*/
void fMatrixFilter(struct Constraints &container)
{
	struct Constraints new_container;
	int num = container.NP;
	int NM;
	int NI;
	Matchespp *t;
	
	for (int i = 0; i < num; i++)
	{
		t = &container.matches[i];

		NM = t->NM;
		NI = fMatrixFilter(container.features[t->idx[0]].keys, container.features[t->idx[1]].keys, t->matches);
		t->NM = NI;
		cout << "FMATRIX : [ " << t->idx[0] << ", " << t->idx[1] << " ] : " <<  NI << " inliers found out of " << NM << endl;

		if(NI < 16) cout << "FMATRIX : [ " << t->idx[0] << ", " << t->idx[1] << " ] : " <<  "Too few matches, considered like if there were no matches" << endl;
		else{new_container.matches.push_back(*t); new_container.NP++;}
	}

	container.assignMatches(new_container);
}

//return : number of inliers
int fMatrixFilter(const vector<KeyPoint> &keys1, const vector<KeyPoint> &keys2, vector<DMatch> &list, float treshold)
{
	Mat fMatrix;

	int nummatch = list.size();

	vector<Point2f> pts1, pts2;

	Mat mask;

	vector<DMatch> new_list;

	int NI = 0;

	if(!treshold) printf("\nPoint : \n");
	 
	for(int i = 0; i < nummatch; i++)
	{
		pts1.push_back(Point2f(keys1[list[i].queryIdx].pt.x, keys1[list[i].queryIdx].pt.y));
		pts2.push_back(Point2f(keys2[list[i].trainIdx].pt.x, keys2[list[i].trainIdx].pt.y));

		//if(!treshold) printf("(%f, %f) ; (%f, %f)\n", pts1[i].x, pts1[i].y, pts2[i].x, pts2[i].y);
		//printf("(%f, %f) ; (%f, %f)\n", pts1[i].x, pts1[i].y, pts2[i].x, pts2[i].y);
	}

	if(!treshold) printf("\n");

	//cout << fMatrix << endl;

	//exit(1);

	fMatrix = findFundamentalMat(pts1, pts2, FM_RANSAC, 3.0, 0.99, mask);

	for (int i = 0; i < nummatch; i++)
	{
		if(mask.at<uchar>(i))
		{
			new_list.push_back(list[i]);
			NI++;
		}
	}

	list = new_list;
	
	return NI;
}


/* 
*	2 Functions : transformInfo
*	Description : Find Transform, compute inliers and its ratio
*	
*	container : container for sift keypoints and their descriptor
*/
void transformInfo(struct Constraints &container)
{
	int num = container.NP;
	Matchespp *t;
	int j = 0;

	for (int i = 0; i < num; i++)
	{
		t = &container.matches[i];
		transformInfo(container.features[t->idx[0]].keys, container.features[t->idx[1]].keys, *t);
		cout << "TRANSFORM : [ " << t->idx[0] << ", " << t->idx[1] << " ] : " << t->NI << " inliers found out of " << t->NM << endl;

		if(t->NI < 10) 
		{
			cout << "TRANSFORM : [ " << t->idx[0] << ", " << t->idx[1] << " ] : " <<  "Too few matches, considered like if there were no matches" << endl;
			t->reset(false);
			j++;
		}
	}

	container.NT = num - j;

	cout << "Good Transform Pairs : " << container.NT << endl;
}

void transformInfo(const vector<KeyPoint> &keys1, const vector<KeyPoint> &keys2, struct Matchespp &list, float treshold)
{
	int nummatch = list.matches.size();

	vector<Point2f> pts1, pts2;

	Mat mask;
	 
	for(int i = 0; i < nummatch; i++)
	{
		pts1.push_back(Point2f(keys1[list.matches[i].queryIdx].pt.x, keys1[list.matches[i].queryIdx].pt.y));
		pts2.push_back(Point2f(keys2[list.matches[i].trainIdx].pt.x, keys2[list.matches[i].trainIdx].pt.y));
	}

	list.H = findHomography(pts1, pts2, CV_RANSAC, treshold, mask);

	list.NI = 0;

	for (int i = 0; i < nummatch; i++)
	{
		if(mask.at<uchar>(i))
		{
			list.NI++;
		}
	}

	list.ratio = (float) list.NI / (float) list.NM;
}

/* 
*	Function : writeConstraints
*	Description : write constraints information in a file for the modified bundlerSFM
*	
*	container : information on matches
*	NP : number of good pairs
*	NT : number of good transform pairs
*/
void writeConstraints(const string &path, const vector<struct Matchespp> &container, int NP, int NT)
{
	string file1(path);
	string file2(path);

	file1.append("matches.init.txt");
	file2.append("ulavalSFM.txt");

	FILE *f1 = fopen(file1.c_str(), "wb");
	FILE *f2 = fopen(file2.c_str(), "wb");

	fprintf(f2, "%d\n", NT);

	for (int i = 0; i < NP; i++)
	{
		fprintf(f1, "%d %d\n", container[i].idx[0], container[i].idx[1]);
		fprintf(f1, "%d\n", container[i].NM);

		for(int j = 0; j < container[i].NM; j++)
		{
			fprintf(f1, "%d %d\n", container[i].matches[j].queryIdx, container[i].matches[j].trainIdx);
		}

		if(container[i].NI > 0)
		{
			fprintf(f2, "%d %d\n", container[i].idx[0], container[i].idx[1]);

	        fprintf(f2, "%d\n", container[i].NI);
	        fprintf(f2, "%f\n", container[i].ratio);

	        const double *M = container[i].H.ptr<double>();

	        fprintf(f2, "%f %f %f %f %f %f %f %f %f\n", M[0], M[1], M[2], M[3], M[4], M[5], M[6], M[7], M[8]);
		}
	}


	fclose(f1);
	fclose(f2);
}

/* 
*	Function : geometry1Core
*	Description : compute geometry on 1 core
*	
*	dir : directory information
*/
void geometry1Core(const util::Directory &dir)
{
	double the_time;
	struct Constraints container;

	cout << "--> Loading Keypoints ..." << endl << endl;
	readKeys(dir, container);
	cout << "--> Loading Matches ..." << endl << endl;
	readMatchFile(dir.getPath(), container);

	cout << endl;

	cout << "--> Prune double matches ..." << endl << endl;
	pruneDoubleMatch(container);
	cout << "--> Filter using fundamental matrix ..." << endl << endl;
	fMatrixFilter(container);
	cout << "--> Compute transform information ..." << endl << endl;
	transformInfo(container);

	cout << "--> Write the information ..." << endl << endl;
	writeConstraints(dir.getPath(), container.matches, container.NP, container.NT);

	cout << endl;
}

/* 
*	Function : geometryMCore
*	Description : start to compute geometry with OpenMPI on the given number of cores
*	
*	path : working directory
*	numcore : number of cores
*/
void geometryMCore(const string &path, int numcore)
{
	stringstream c;

	c << "mpirun -n " << numcore << " cDoGeometry " << path;

	string command = c.str();

	system(command.c_str());
}

/* 
*	Function : SiftCMCore
*	Description : start to compute geometry with OpenMPI on the given number of cores and on the supercomputer
*	
*	path : working directory
*	numcore : number of cores
*/
void geometryMCCore(const string &path, int numcore)
{
	stringstream c;

	c << "msub submitGeo.sh";

	string command = c.str();

	system(command.c_str());

	cout << "You'll be warned by email when the process will terminate" << endl << endl;
}




























