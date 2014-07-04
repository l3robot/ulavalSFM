/*
*	File : backup.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
*	Version : 1.0
*	
*	Description : Backup function
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

#include "dosift.h"

unsing namespace std;
unsing namespace cv;


/* 
*	Function : distancePL
*	Description : calculate the distance between a point and a line
*	
*	point : a floating point
*	line : a line
*
*	return : distance between the point and the line
*/
float distancePL(float point[2], Mat line)
{
	return (abs(line.at<float>(0, 0) * point[0] + line.at<float>(0, 1) * point[1] + line.at<float>(0, 2)) / sqrt(line.at<float>(0, 0) * line.at<float>(0, 0) + line.at<float>(0, 1) * line.at<float>(0, 1)));
}



/* 
*	Function : doMatch
*	Description : match an image pair
*	
*	img1 : container with img1 sift information
*	img2 : container with img2 sift information
*	container : matches information
*
*	Backup description : Contains a part with fMatrix and is relative inlier filter
*/
void doMatch(const SFeatures &img1, const SFeatures &img2, Matches &container)
{
	FlannBasedMatcher matcher;
	vector<vector<DMatch> > two_matches;
	vector<DMatch> matches;
	float* pter;

	matcher.knnMatch(keys, keys2, two_matches, 2);

	for(int i = 0; i < keys.rows; i++)
	{
		if(two_matches[i][0].distance < 0.8 * two_matches[i][1].distance)
		{
			matches.push_back(two_matches[i][0]);
		}
	}

	if(matches.size() < 8)
	{
		container.matches.clear();
	}
	else
	{
		Mat pts1(matches.size(), 2, CV_32F), pts2(matches.size(), 2, CV_32F);

		for(int i = 0; i < matches.size(); i++)
		{
			pter = pts1.ptr<float>(i);
			pter[0] = img1.keys[matches[i].queryIdx].y;
			pter[1] = img1.keys[matches[i].queryIdx].x;
			pter = pts2.ptr<float>(i);
			pter[0] = img2.keys[matches[i].trainIdx].y;
			pter[1] = img2.keys[matches[i].trainIdx].x;
		}

		container.fMatrix = findFundamentalMat(pts1, pts2);

		vector<Point2f> point;
		Mat line(1, 3, CV_32F);

		float distance = 0.0;

		for(int i = 0; i < matches.size(); i++)
		{
			point.push_back(Point2f(t_pts1.at<float>(i, 0), t_pts1.at<float>(i, 1)));

			computeCorrespondEpilines(point, 1, fMatrix, line);

			distance = distancePL(t_pts2.ptr<float>(i), line);

			if(distance < 3.0)
			{
				container.NM++;
				container.matches.push_back(matches.begin() + i);
			}
		}
	}
}