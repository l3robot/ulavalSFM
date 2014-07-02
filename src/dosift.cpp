/*
*	File : dosift.cpp
*	Author : Louis-Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*	
*	Description : Functions relative to sift
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

#include "directory.h"
#include "dosift.h"

using namespace std;
using namespace cv;


/* 
*	Function : doSift
*	Description : Find sift points on the image
*	
*	path : path of the image
*	container : container for sift keypoints and their descriptor
*/
void doSift(const string &path, struct SFeatures &container)
{
	Mat img, des;
	vector<KeyPoint> keypoints;

	img = imread(path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

	SiftFeatureDetector detector; 

   	detector.detect(img, keypoints);

   	SiftDescriptorExtractor extractor;

    extractor.compute(img, keypoints, des);

    container.des = des;
    container.keys = keypoints;
}

/* 
*	Function : WriteSiftFile
*	Description : write the keypoints and their descriptor in the sift files (Lowe's binairy format)
*	
*	file : path of the .sift file
*	container : container for sift keypoints and their descriptor
*/
void writeSiftFile(const string &file, const struct SFeatures &container)
{
	FILE* f = fopen(file.c_str(), "wb");

    fprintf(f, "%d %d \n", container.des.rows, container.des.cols);

    for(int i = 0; i < container.keys.size(); i++)
    {
      	fprintf(f, "%f %f %f %f \n", container.keys.at(i).pt.y, container.keys.at(i).pt.x, container.keys.at(i).size, (container.keys.at(i).angle*M_PI/180.0));
       	for(int j = 0; j < 128; j++)
       	{
       	fprintf(f, "%d ", (int)container.des.at<float>(i,j));
       	}
       	fprintf(f, "\n");
    }

    fclose(f);
}

/* 
*	Function : Sift1Core
*	Description : Find sift points and write those in files
*	
*	dir : directory information
*/
void sift1Core(const util::Directory &dir)
{
	double the_time;
	struct SFeatures container;
	string file(dir.getPath());

	cout << endl;

	for(int i = 0; i < dir.getNBImages(); i++)
	{
		file.append(dir.getImage(i));

		doSift(file, container);

		cout << container.keys.size() << " sift point(s) found in " << dir.getImage(i) << endl;

		while (file[file.size() - 1] != '.')
		{
			file.pop_back();
		}

		file.append("sift");

		writeSiftFile(file, container);

		while (file[file.size() - 1] != '/')
		{
			file.pop_back();
		}
	}

	cout << endl;
}

/* 
*	Function : SiftMCore
*	Description : start to find sift point with OpenMPI on the given number of cores
*	
*	path : working directory
*	numcore : number of cores
*/
void siftMCore(const string &path, int numcore)
{
	stringstream c;

	c << "mpirun -n " << numcore << " ./cDoSift " << path;

	string command = c.str();

	system(command.c_str());
}

/* 
*	Function : SiftCMCore
*	Description : start to find sift point with OpenMPI on the given number of cores and on the supercomputer
*	
*	path : working directory
*	numcore : number of cores
*/
void siftMCCore(const string &path, int numcore)
{
	stringstream c;

	c << "msub submitSIFT.sh";

	string command = c.str();

	system(command.c_str());

	cout << "You'll be warned by email when the process will terminate" << endl << endl;
}


