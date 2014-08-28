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

#include "util.h"
#include "domatch.h"
#include "dosift.h"
#include "directory.h"
#include "dogeometry.h"

using namespace std;
using namespace cv;


/* 
*	Function : listDir
*	Description : to make a list of .key file path
*	
*	dir : directory information
*	list : vector of path which will be created
*/
void listDir(const util::Directory &dir, vector<string> &list)
{
	string path(dir.getPath());

	int numimages = dir.getNBImages();

	for(int i = 0; i < numimages; i++)
	{
		path.append(dir.getImage(i));
		while(path[path.size() - 1] != '.') path.pop_back();
		path.append("key");
		list.push_back(path);
		while(path[path.size() - 1] != '/') path.pop_back();
	}
}

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

	fscanf(f, "%d %f", &container.NF, &garbage);

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
*	geo : if to do geometry or not
*	container : ratio for the distance filter
*
*	return : 1 if NI > 0, else 0
*/
int doMatch(const SFeatures &img1, const SFeatures &img2, Matchespp &container, int geo, float ratio)
{
	FlannBasedMatcher matcher(new flann::KDTreeIndexParams(16), new flann::SearchParams(64));
	vector<vector<DMatch> > two_matches;

	matcher.knnMatch(img2.des, img1.des, two_matches, 2);

	for(int i = 0; i < img2.des.rows; i++)
	{
		if(two_matches[i][0].distance < ratio * two_matches[i][1].distance)
		{
			container.NM++;
			container.matches.push_back(two_matches[i][0]);
		}
	}

	if (geo)
	{

		//Prune double matches
		struct Matchespp new_container(container.idx);
		
		pruneDoubleMatch(new_container, container);

		if (new_container.NM >= 20) container.assign(new_container);
		else container.reset();
		///////////////////////

		//FMatrix filter
		if (container.NM > 0)
		{
			int NI = fMatrixFilter(img2.keys, img1.keys, container.matches);

			//cout << "FMATRIX : [ " << container.idx[0] << ", " << container.idx[1] << " ] : " << NI << " inliers found out of " << container.NM << endl;

			if (NI >= 16) container.NM = NI;
			else container.reset();
		}
		////////////////

		//Transform info
		if (container.NM > 0)
		{
			transformInfo(img2.keys, img1.keys, container);

			//cout << "TRANSFORM : [ " << container.idx[0] << ", " << container.idx[1] << " ] : " << container.NI << " inliers found out of " << container.NM << endl;

			if (container.NI < 10){ container.reset(false); return 0;}

			return 1;
		}
		////////////////

	}

	return 0;
}

/* 
*	Function : findIDX
*	Description : find idx of a match
*	
*	i : idx of image 1
*	j : idx of image 2
*	container : vector filled with all the matches
*	reverse : indicate if the images idx are in order or reversed
*
*	return : -1 if not found, idx if found
*/
int findIDX(int i, int j, const vector<struct Matchespp> &container, int* reverse)
{
	int num = container.size();

	*reverse = 0;

	for(int a = 0; a < num; a++)
	{
		if((int) container[a].idx[0] == i && (int) container[a].idx[1] == j)
			return a;
		if((int) container[a].idx[0] == j && (int) container[a].idx[1] == i)
		{
			*reverse = 1;
			return a;
		}
	}

	return -1;
}


/* 
*	Function : writeMatchFile
*	Description : write matches in a file
*	
*	path : path of the working directory
*	container : container with matches information
*	n : numbre of images
*	bar : print or not a progress bar
*/
void writeMatchFile(const string &path, const vector<struct Matchespp> &container, int n, int bar)
{
	string file(path);

	file.append("matches.init.txt");

	FILE* f = fopen(file.c_str(), "wb");

	int ni = ( 1 + sqrt( 1 + 8 * n ) ) / 2;

	for (int i = 0; i < ni; i++)
	{
		for (int j = 0; j < ni; j++)
		{
			int reverse;
			int idx = findIDX(i, j, container, &reverse);

			if (idx > 0 && !reverse)
			{
				struct Matchespp box(container[idx]);

				fprintf(f, "%d %d\n", box.idx[0], box.idx[1]);
				fprintf(f, "%d\n", box.NM);

				for(int j = 0; j < box.NM; j++)
					fprintf(f, "%d %d\n", box.matches[j].queryIdx, box.matches[j].trainIdx);
			}
			else if (idx > 0 && reverse)
			{
				struct Matchespp box(container[idx]);

				fprintf(f, "%d %d\n", box.idx[1], box.idx[0]);
				fprintf(f, "%d\n", box.NM);

				for(int j = 0; j < box.NM; j++)
					fprintf(f, "%d %d\n", box.matches[j].trainIdx, box.matches[j].queryIdx);
			}

			if (bar) showProgress(i * ni + j, ni * ni, 75, 1);

		}
	}

	if (bar) showProgress(ni * ni, ni * ni, 75, 0);

	fclose(f);
}


/* 
*	Function : match1Core
*	Description : match points and write those match in files
*	
*	dir : directory information
*/
void match1Core(const util::Directory &dir, int geo)
{
	double the_time;
	struct SFeatures img1;
	struct SFeatures img2;

	vector<struct Matchespp> v_serialMatch;

	int n = dir.getNBImages() * (dir.getNBImages() - 1) / 2;
	int prog = 0;
	int NT = 0;

	string file(dir.getPath());
	file.append(MATCHFILE);

	FILE* f = fopen(file.c_str(), "wb");

	while (file[file.size() - 1] != '/')
	{
		file.pop_back();
	}

	int NBImages = dir.getNBImages();

	cout << endl;

	cout << "--> Matching : " << endl;
	
	for(int i = 0; i < NBImages; i++)
	{
		file.append(dir.getImage(i));

		while (file[file.size() - 1] != '.')
		{
			file.pop_back();
		}

		file.append("key");

		readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), file, img1);

		while (file[file.size() - 1] != '/')
		{
			file.pop_back();
		}

		for(int j = 0; j < i; j++)
		{
			file.append(dir.getImage(j));

			while (file[file.size() - 1] != '.')
			{
				file.pop_back();
			}

			file.append("key");

			readAndAdjustSiftFile(dir.getPath(), dir.getImage(j), file, img2);

			struct Matchespp container(j, i);

			if (doMatch(img1, img2, container, geo)) NT++;

			//cout << container.NM << " match(es) found between " << dir.getImage(j) << " and " << dir.getImage(i) << endl;
	
			if (container.NM) v_serialMatch.push_back(container);

			while (file[file.size() - 1] != '/')
			{
				file.pop_back();
			}

			prog++;
			showProgress(prog, n, 75, 1);

		}
	}

	showProgress(n, n, 75, 0);

	cout << endl;

	cout << "--> Writing files :" << endl;

	if (geo) writeConstraints(dir.getPath(), v_serialMatch, n);
	else writeMatchFile(dir.getPath(), v_serialMatch, n);

	fclose(f);
}

/* 
*	Function : matchMCore
*	Description : start to match point with OpenMPI on the given number of cores
*	
*	path : working directory
*	numcore : number of cores
*/
void matchMCore(const string &path, int numcore, int geo)
{
	stringstream c;
	if (geo)
		c << "mpirun -n " << numcore << " cDoMatch " << path;
	else
		c << "mpirun -n " << numcore << " cDoMatch " << path << " 1 0";

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
void matchMCCore(const string &path, int numcore, int seconds)
{
	stringstream c;

	printf("--> Create the script : \n");

	createSubmit(path, numcore, seconds, 1);

	printf("--> Launch the script : \n");

	c << "msub ulavalSub/submit.sh";

	string command = c.str();

	system(command.c_str());

	cout << "Process launch, you can enter the command \"watch -n 10 showq -u $USER\" to see the progression." << endl << endl;
}












