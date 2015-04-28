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
*	Function : pruneDoubleMatch
*	Description : Eliminate the double matches, keep the first only. If a pair have less than 20 matches, the pair is eliminated.
*
*/
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
*	Function : fMatrixFilter
*	Description : Estimate FMatrix with RANSAC and keep inliers only
*
*/
int fMatrixFilter(const vector<KeyPoint> &keys1, const vector<KeyPoint> &keys2, vector<DMatch> &list, float treshold)
{
	Mat fMatrix;

	int nummatch = list.size();

	vector<Point2f> pts1, pts2;

	Mat mask;

	vector<DMatch> new_list;

	int NI = 0;

	for(int i = 0; i < nummatch; i++)
	{
		pts1.push_back(Point2f(keys1[list[i].queryIdx].pt.x, keys1[list[i].queryIdx].pt.y));
		pts2.push_back(Point2f(keys2[list[i].trainIdx].pt.x, keys2[list[i].trainIdx].pt.y));
	}

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
*	Function : transformInfo
*	Description : Find Transform, compute inliers and its ratio
*
*/
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

  //Sometimes, the algorithm doesn't find a matrix, so it is important to test it
	for (int i = 0; i < nummatch && !list.H.empty(); i++)
	{
		if(mask.at<uchar>(i))
		{
			list.NI++;
		}
	}

	list.ratio = (float) list.NI / (float) list.NM;
}
