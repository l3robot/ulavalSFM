/*
*	File : domatch.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
*	Version : 1.0
*	
*	Description : Functions relative to match
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "domatch.h"
#include "dosift.h"
#include "directory.h"

#define MATCHFILE "matches.init.txt"

using namespace std;
using namespace cv;

/* 
*	Function : readSiftFile
*	Description : read a .sift file
*	
*	file : path of the .sift file
*	container : container for sift keypoints and their descriptor
*/
void readSiftFile(const string &file, SFeatures &container)
{
	FILE* f = fopen(file.c_str(), "r");

	if(!f)
	{
		cout << "[ERROR] The file " << file << " does not exist" << endl;
		cout << "[ERROR] Maybe you haven't found sift point yet" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	float garbage, *pter;

	fscanf(f, "%d %d", &container.NF, &garbage);

	Mat descriptor(container.NF, 128, CV_32F);
	KeyPoint key;

	for(int i = 0; i < container.NF; i++)
	{
		fscanf(f, "%f %f %f %f", &key.pt.y, &key.pt.x, &garbage, &garbage);

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
*	Function : doMatch
*	Description : match an image pair
*	
*	img1 : container with img1 sift information
*	img2 : container with img2 sift information
*	container : matches information
*/
void doMatch(const SFeatures &img1, const SFeatures &img2, Matches &container, float ratio)
{
	FlannBasedMatcher matcher;
	vector<vector<DMatch> > two_matches;
	float* pter;

	matcher.knnMatch(img1.des, img2.des, two_matches, 2);

	for(int i = 0; i < img1.des.rows; i++)
	{
		if(two_matches[i][0].distance < ratio * two_matches[i][1].distance)
		{
			container.NM++;
			container.matches.push_back(two_matches[i][0]);
		}
	}
}


/* 
*	Function : writeMatch
*	Description : write match in a file
*	
*	f : file descriptor
*	container : container with matches information
*	i : index i
*	j : index j
*/
void writeMatchFile(FILE* f, const Matches &container, int i, int j)
{
	fprintf(f, "%d %d\n", i, j);
	fprintf(f, "%d\n", container.NM);

	for(int i = 0; i < container.NM; i++)
	{
		fprintf(f, "%d %d\n", container.matches[i].queryIdx, container.matches[i].trainIdx);
	}
}


/* 
*	Function : match1Core
*	Description : match points and write those match in files
*	
*	dir : directory information
*/
void match1Core(const util::Directory &dir)
{
	double the_time;
	struct SFeatures img1;
	struct SFeatures img2;
	struct Matches container;

	string file(dir.getPath());
	file.append(MATCHFILE);

	FILE* f = fopen(file.c_str(), "wb");

	while (file[file.size() - 1] != '/')
	{
		file.pop_back();
	}

	int NBImages = dir.getNBImages();

	cout << endl;

	for(int i = 0; i < NBImages; i++)
	{
		file.append(dir.getImage(i));

		while (file[file.size() - 1] != '.')
		{
			file.pop_back();
		}

		file.append("key");

		readSiftFile(file, img1);

		while (file[file.size() - 1] != '/')
		{
			file.pop_back();
		}

		for(int j = i + 1; j < NBImages; j++)
		{
			file.append(dir.getImage(j));

			while (file[file.size() - 1] != '.')
			{
				file.pop_back();
			}

			file.append("key");

			readSiftFile(file, img2);

			doMatch(img1, img2, container);

			cout << container.NM << " match(es) found between " << dir.getImage(i) << " and " << dir.getImage(j) << endl;
	
			writeMatchFile(f, container, i, j);

			container.reset();

			while (file[file.size() - 1] != '/')
			{
				file.pop_back();
			}
		}
	}

	cout << endl;

	fclose(f);
}

/* 
*	Function : matchMCore
*	Description : start to match point with OpenMPI on the given number of cores
*	
*	path : working directory
*	numcore : number of cores
*/
void matchMCore(const string &path, int numcore)
{
	stringstream c;

	c << "mpirun -n " << numcore << " ./cDoMatch " << path;

	string command = c.str();

	system(command.c_str());
}

/* 
*	Function : matchCMCore
*	Description : start to match point with OpenMPI on the given number of cores and on the supercomputer
*	
*	path : working directory
*	numcore : number of cores
*/
void matchMCCore(const string &path, int numcore)
{
	stringstream c;

	c << "msub submitMatch.sh";

	string command = c.str();

	system(command.c_str());

	cout << "You'll be warned by email when the process will terminate" << endl << endl;
}












