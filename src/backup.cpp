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













int fMatrixFilter(const vector<KeyPoint> &keys1, const vector<KeyPoint> &keys2, vector<DMatch> &list, int treshold)
{
	Mat fMatrix(3, 3, CV_32F);

	Mat pts1(matches.size(), 2, CV_32F), pts2(matches.size(), 2, CV_32F);

	Mat pt(1, 2, CV_32F);

	Mat line(1, 3, CV_32F);

	Mat mask;

	float *pter, distance;

	vector<DMatch> new_list;

	int NI = 0;
	 
	for(int i = 0; i < list.matches.size(); i++)
	{
		pter = pts1.ptr<float>(i);
		pter[0] = keys1[list.matches[i].queryIdx].pt.x;
		pter[1] = keys1[list.matches[i].queryIdx].pt.y;
		pter = pts2.ptr<float>(i);
		pter[0] = keys2[list.matches[i].trainIdx].pt.x;
		pter[1] = keys2[list.matches[i].trainIdx].pt.y;
	}

	fMatrix = findFundamentalMat(pts1, pts2, FM_RANSAC, 3.0, 0.99, mask);

	for (int i = 0; i < list.matches.size(); i++)
	{
		pter = pt.ptr<float>(0);
		pter[0] = pts1.at<float>(i, 0);
		pter[1] = pts1.at<float>(i, 1);

		computeCorrespondEpilines(pt, 1, fMatrix, line);

		distance = distancePL(pt, line);

		if(distance <= treshold)
		{
			new_list.push_back(list.matches[i]);
			NI++;
		}
	}

	list = new_list;
	return NI;
}




/* 
*	Function : findDistance
*	Description : First, calculate the hypothetical point 2 using H matrix, then calculate the distance between a the hypothetical point 2 and the true point 2
*	
*	pts1 : point 1
*	pts2 : point 2
*	H : H matrix
*
*	return : distance between the hypothetical point 2 and the true point 2
*/
float findDistance(float pts1[2], float pts2[2], const Mat &H)
{
	float pts[3];

	const double* pter = H.ptr<double>(0);
	pts[0] = (float) pter[0] * pts1[0] + (float) pter[1] * pts1[1] + (float) pter[2];
	pter = H.ptr<double>(1);
	pts[1] = (float) pter[0] * pts1[0] + (float) pter[1] * pts1[1] + (float) pter[2];
	pter = H.ptr<double>(2);
	pts[2] = (float) pter[0] * pts1[0] + (float) pter[1] * pts1[1] + (float) pter[2];

	//cout << "New : " << pts[0]/pts[2] << "|" << pts[1]/pts[2] << "|" << pts[2]/pts[2] << endl;
	//cout << "Old : " << pts2[0] << "|" << pts2[1] << "|" << 1 << endl;

	float x = pts2[0] - pts[0]/pts[2];
	float y = pts2[1] - pts[1]/pts[2];

	//cout << "x : " << x << endl;
	//cout << "y : " << y << endl;
	//cout << "Distance : " << (x*x + y*y) << endl;

	return (x*x + y*y);
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
	
			if (container.NM >= 16) writeMatchFile(f, container, i, j);

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


int fMatrixFilter(const vector<KeyPoint> &keys1, const vector<KeyPoint> &keys2, vector<DMatch> &list, float treshold)
{
	Mat fMatrix;

	int nummatch = list.size();

	vector<Point2f> pts1, pts2;

	Mat line;

	float distance;

	vector<DMatch> new_list;

	int NI = 0;

	if(!treshold) printf("\nPoint : \n");
	 
	for(int i = 0; i < nummatch; i++)
	{
		pts1.push_back(Point2f(keys1[list[i].queryIdx].pt.x, keys1[list[i].queryIdx].pt.y));
		pts2.push_back(Point2f(keys2[list[i].trainIdx].pt.x, keys2[list[i].trainIdx].pt.y));

		if(!treshold) printf("(%f, %f) ; (%f, %f)\n", pts1[i].x, pts1[i].y, pts2[i].x, pts2[i].y);
	}

	if(!treshold) printf("\n");

	fMatrix = findFundamentalMat(pts1, pts2, FM_RANSAC, 3.0, 0.99);

	//cout << "elemsize line before : " << line.elemSize() << endl;

	//computeCorrespondEpilines(pts1, 1, fMatrix, line);

	cout << fMatrix << endl;

	//cout << "elemsize fMatrix : " << fMatrix.elemSize() << endl;
	//cout << "elemsize line : " << line.elemSize() << endl;

	for (int i = 0; i < nummatch; i++)
	{
		//distance = distancePL(pts2.ptr<float>(i), line.ptr<float>(i));	
		float r[2] = {pts2[i].x, pts2[i].y};
		float l[2] = {pts1[i].x, pts1[i].y};

		distance = (float) distancePL(fMatrix.ptr<double>(), r, l);

		//cout << distance << endl;

		if(distance <= 3.0) //treshold
		{
			new_list.push_back(list[i]);
			NI++;
		}
		
	}

	list = new_list;
	
	return NI;
}


/* 
*	Function : distancePL
*	Description : calculate the distance between a point and a line
*	
*	point : a floating point
*	line : a line
*
*	return : distance between the point and the line
*/
float distancePL(float point[2], float line[3])
{
	return (abs(line[0] * point[0] + line[1] * point[1] + line[2]) / sqrt(line[0] * line[0] + line[1] * line[1]));
}

double distancePL(double *F, float r[2], float l[2]) {
    double Fl[3], Fr[3], pt;    

#if 1
    Fl[0] = F[0] * (double) l[0] + F[1] * (double) l[1] + F[2];
    Fl[1] = F[3] * (double) l[0] + F[4] * (double) l[1] + F[5];
    Fl[2] = F[6] * (double) l[0] + F[7] * (double) l[1] + F[8];

    Fr[0] = F[0] * (double) r[0] + F[3] * (double) r[1] + F[6];
    Fr[1] = F[1] * (double) r[0] + F[4] * (double) r[1] + F[7];
    Fr[2] = F[2] * (double) r[0] + F[5] * (double) r[1] + F[8];

    pt = (double) r[0] * Fl[0] + (double) r[1] * Fl[1] + Fl[2];
#else
    matrix_product(3, 3, 3, 1, F, l.p, Fl);
    matrix_transpose_product(3, 3, 3, 1, F, r.p, Fr);
    matrix_product(1, 3, 3, 1, r.p, Fl, &pt);
#endif

    return
	(1.0 / (Fl[0] * Fl[0] + Fl[1] * Fl[1]) +
	 1.0 / (Fr[0] * Fr[0] + Fr[1] * Fr[1])) *
	(pt * pt);
}

