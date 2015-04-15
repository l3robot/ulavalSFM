/*
*	File : libgeometry.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2015, March 7th
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
#include <opencv2/features2d/features2d.hpp>
#if CV_VERSION_MAJOR == 2
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#elif CV_VERSION_MAJOR == 3
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#endif

#include "util.h"
#include "libmatch.h"
#include "libgeometry.h"
#include "directory.h"

using namespace std;
using namespace cv;

/* CV_LOAD_IMAGE_GRAYSCALE is renamed to IMREAD_GRAYSCALE in OpenCV 3 */
#if CV_VERSION_MAJOR == 3
    using namespace xfeatures2d;

    #define CV_LOAD_IMAGE_GRAYSCALE IMREAD_GRAYSCALE
    #include <opencv2/calib3d/calib3d_c.h>
#endif


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

	for (int i = 0; i < num; i++)
	{
		int NM;
		int NI;
		Matchespp *t;

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
	int j = 0;

	for (int i = 0; i < num; i++)
	{
		Matchespp *t;

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
