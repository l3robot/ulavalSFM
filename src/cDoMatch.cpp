/*
*	File : cDoSift.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
*	Version : 1.0
*	
*	Description : Program to make match in parallel
*/





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <time.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>

#define VISUALSFM

#ifdef VISUALSFM
#include "MatchFile/MatchFile.h"
#include "MatchFile/points.h"
#include "5point.h"
#endif

using namespace std;
using namespace cv;


double distancePL(float* point, Mat line)
{
	return (abs(line.at<float>(0, 0) * point[0] + line.at<float>(0, 1) * point[1] + line.at<float>(0, 2)) / sqrt(line.at<float>(0, 0) * line.at<float>(0, 0) + line.at<float>(0, 1) * line.at<float>(0, 1)));
}

#ifdef LAVALSFM
vector<DMatch> doMatch(Mat keys, Mat keys2, Mat pts1, Mat pts2)
{
	FlannBasedMatcher matcher;
	vector<vector<DMatch> > two_matches;
	vector<DMatch> matches;
	float* pter;

	matcher.knnMatch(keys, keys2, two_matches, 2);

	for(int i = 0; i < keys.rows; i++)
	{
		if(two_matches[i][0].distance < 0.6 * two_matches[i][1].distance)
		{
			matches.push_back(two_matches[i][0]);
		}
	}

	//RANSAC PART
#ifdef RANSAC
	if(matches.size() < 8)
	{
		matches.clear();
	}
	else
	{
		Mat t_pts1(matches.size(), 2, CV_32F), t_pts2(matches.size(), 2, CV_32F);

		for(int i = 0; i < matches.size(); i++)
		{
			pter = t_pts1.ptr<float>(i);
			pter[0] = pts1.at<float>(matches[i].queryIdx, 0);
			pter[1] = pts1.at<float>(matches[i].queryIdx, 1);
			pter = t_pts2.ptr<float>(i);
			pter[0] = pts2.at<float>(matches[i].trainIdx, 0);
			pter[1] = pts2.at<float>(matches[i].trainIdx, 1);
		}

		Mat fMatrix = findFundamentalMat(t_pts1, t_pts2);
		vector<Point2f> point;
		Mat line(1, 3, CV_32F);
		double distance = 0.0;

		for(int i = 0; i < matches.size(); i++)
		{
			point.push_back(Point2f(t_pts1.at<float>(i, 0), t_pts1.at<float>(i, 1)));

			computeCorrespondEpilines(point, 1, fMatrix, line);

			distance = distancePL(t_pts2.ptr<float>(i), line);

			if(distance > 3.0)
			{
				matches.erase(matches.begin() + i);
			}
		}
	}
#endif
	return matches;
}
#endif

#ifdef LAVALSFM
void writeMatch(FILE* f, vector<DMatch> matches, char* img1, char* img2)
{
	char* pter1 = strrchr(img1, '.');
	char* pter2 = strrchr(img2, '.');

	pter1 = strcpy(pter1, ".jpg");
	pter2 = strcpy(pter2, ".jpg");

	pter1 = strrchr(img1, '/');
	pter2 = strrchr(img2, '/');

	fprintf(f, "%s %s %lu\n", pter1+1, pter2+1, matches.size());

	pter1 = strrchr(img1, '.');
	pter2 = strrchr(img2, '.');

	pter1 = strcpy(pter1, ".sift");
	pter2 = strcpy(pter2, ".sift");

	for(int i = 0; i < matches.size(); i++)
	{
		fprintf(f, "%d ", matches[i].queryIdx);
	}
	fprintf(f, "\n");
	for(int i = 0; i < matches.size(); i++)
	{
		fprintf(f, "%d ", matches[i].trainIdx);
	}
	fprintf(f, "\n");
}
#endif

#ifdef LAVALSFM
int main(int argc, char** argv)
{
	char** list;
	int nb_lines;
	double time;

	Mat keys, keys2, pts1, pts2;

	vector<DMatch> matches;

	clock_t start = clock();

	FILE* f = fopen("matches.mat", "w");

	for(int path = 1; path < argc; path++)
	{
		list = list_dir(argv[path], &nb_lines);

		for(int i = 0; i < nb_lines; i++)
		{
			keys = readSiftFile(list[i], pts1);

			for(int j = i + 1; j < nb_lines; j++)
			{
				keys2 = readSiftFile(list[j], pts2);
				matches = doMatch(keys, keys2, pts1, pts2);
				writeMatch(f, matches, list[i], list[j]);
				printf("%08lu appariements trouvés entre l'image %s et l'image %s\n", matches.size(), list[i], list[j]);
			}
		}

		free(list);
	}

	time = (clock() - start) / (double) CLOCKS_PER_SEC;

	printf("Le programme a pris environ %f secondes\n", time);

	fclose(f);

	return 0;
}
#endif

#ifdef VISUALSFM
Mat doMatch(Mat keys, Mat keys2, Mat pts1, Mat pts2, Mat& eMatrix, int** index1, int** index2, int* NF)
{
	FlannBasedMatcher matcher;
	vector<vector<DMatch> > two_matches;
	vector<DMatch> matches;
	vector<DMatch> good_matches;
	float* pter;

	matcher.knnMatch(keys, keys2, two_matches, 2);

	for(int i = 0; i < keys.rows; i++)
	{
		if(two_matches[i][0].distance < 0.6 * two_matches[i][1].distance)
		{
			matches.push_back(two_matches[i][0]);
		}
	}
	//RANSAC PART

	Mat fMatrix(3, 3, CV_32F);

	if(matches.size() < 8)
	{
		good_matches.clear();
	}
	else
	{
		Mat t_pts1(matches.size(), 2, CV_32F), t_pts2(matches.size(), 2, CV_32F);

		for(int i = 0; i < matches.size(); i++)
		{
			pter = t_pts1.ptr<float>(i);
			pter[0] = pts1.at<float>(matches[i].queryIdx, 0);
			pter[1] = pts1.at<float>(matches[i].queryIdx, 1);
			pter = t_pts2.ptr<float>(i);
			pter[0] = pts2.at<float>(matches[i].trainIdx, 0);
			pter[1] = pts2.at<float>(matches[i].trainIdx, 1);
		}

		fMatrix = findFundamentalMat(t_pts1, t_pts2);
		//eMatrix = findEssentialMat(t_pts1, t_pts2);
		vector<Point2f> point;
		Mat line(1, 3, CV_32F);
		double distance = 0.0;

		for(int i = 0; i < matches.size(); i++)
		{
			point.push_back(Point2f(t_pts1.at<float>(i, 0), t_pts1.at<float>(i, 1)));

			computeCorrespondEpilines(point, 1, fMatrix, line);

			distance = distancePL(t_pts2.ptr<float>(i), line);

			if(distance > 3.0)
			{
				good_matches.push_back(matches[i]);
			}
		}
	}
	*NF = matches.size();

	*index1 = (int *) malloc(matches.size()*sizeof(int));
	*index2 = (int *) malloc(matches.size()*sizeof(int));

	for (int i = 0; i < matches.size(); i++)
	{
		(*index1)[i] = matches[i].queryIdx;
		(*index2)[i] = matches[i].trainIdx;
	}
	return fMatrix;
}
#endif

#ifdef VISUALSFM
void writeMatch(Mat fMatrix, Mat eMatrix, int* index1, int* index2, int fc1, int fc2, int NF, char* img1, char* img2)
{
	float F[3][3];
	float R[3][3];
	float T[3];

	char* pter1 = strrchr(img1, '.');
	char* pter2 = strrchr(img2, '.');

	pter1 = strcpy(pter1, "\0");
	pter2 = strcpy(pter2, "\0");

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			F[i][j] = fMatrix.at<float>(i,j);
		}
	}

	/*for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = eMatrix.at<float>(i,j);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		T[i] = eMatrix.at<float>(3,j);
	}*/

	int* indices [2] = {index1, index2};
	Points<int> inliers(indices, NF, 2);

	//TwoViw

	MatchFile::WritePMatch(img1, img2, fc1, fc2, NF, inliers);
	MatchFile::WriteIMatch(img1, img2, NF, index1, index2, F);

	img1 = strcat(img1, ".sift");
	img2 = strcat(img2, ".sift");
}
#endif

#ifdef VISUALSFM
int main(int argc, char** argv)
{
	char** list;
	int nb_lines;
	double time;
	int *index1, *index2;
	int NF, fc1, fc2;

	Mat keys, keys2, pts1, pts2;

	Mat fMatrix;

	Mat eMatrix(3, 4, CV_32F);

	clock_t start = clock();

	for(int path = 1; path < argc; path++)
	{
		list = list_dir(argv[path], &nb_lines);

		for(int i = 0; i < nb_lines; i++)
		{
			keys = readSiftFile(list[i], pts1, &fc1);

			for(int j = i + 1; j < nb_lines; j++)
			{
				keys2 = readSiftFile(list[j], pts2, &fc2);
				fMatrix = doMatch(keys, keys2, pts1, pts2, eMatrix, &index1, &index2, &NF);
				writeMatch(fMatrix, index1, index2, fc1, fc2, NF, list[i], list[j]);
				free(index1);
				free(index2);
				printf("%08d appariements trouvés entre l'image %s et l'image %s\n", NF, list[i], list[j]);
			}
		}

		for(int i = 0; i < nb_lines; i++)free(list[i]);

		free(list);
	}

	time = (clock() - start) / (double) CLOCKS_PER_SEC;

	printf("Le programme a pris environ %f secondes\n", time);

	return 0;
}
#endif













