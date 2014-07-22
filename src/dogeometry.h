/*
*	File : dogeometry.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/08/2014
*	Version : 1.0
*	
*	Description : Functions relative to geometry
*/

#ifndef DOGEOMETRY
#define DOGEOMETRY

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "dosift.h"
#include "directory.h"

#define GOEMETRYFILE "constraints.txt"


/* 
*	Struct : Matchespp
*	Description : More information on matches
*
*	int idx[2] : images index
*	int NM : number of matches
*	int NI : number of inliers relative to transform
*	int H[9] : transform matrix
*	int ratio : pairwise score	
*	std::vector<DMatch> matches : matches
*/
struct Matchespp
{
	int idx[2];
	int NM;
	int NI;
	cv::Mat H;
	float ratio;

	std::vector<cv::DMatch> matches;

	Matchespp()
	{
		idx[0] = 0;
		idx[1] = 0;
		NM = 0;
		H = cv::Mat(3, 3, CV_32F);
		ratio = 0.0;
	}

	Matchespp(int new_idx[2])
	{
		idx[0] = new_idx[0];
		idx[1] = new_idx[1];
		NM = 0;
		H = cv::Mat(3, 3, CV_32F);
		ratio = 0.0;
	}

	Matchespp(const Matchespp &new_matches)
	{
		idx[0] = new_matches.idx[0];
		idx[1] = new_matches.idx[1];
		NM = new_matches.NM;
		H = new_matches.H;
		ratio = new_matches.ratio;
		matches = new_matches.matches;
	}

	void assign(const Matchespp &new_matches)
	{
		idx[0] = new_matches.idx[0];
		idx[1] = new_matches.idx[1];
		NM = new_matches.NM;
		H = new_matches.H;
		ratio = new_matches.ratio;
		matches = new_matches.matches;
	}

	void reset(bool clearmatches = true)
	{
		NI = 0;
		H = cv::Mat(3, 3, CV_32F);
		ratio = 0.0;
		if(clearmatches){matches.clear(); NM = 0;}
	}

	void print()
	{
		std::cout << std::endl << idx[0] << ":" << idx[1] << " [" << std::endl;
		for(int i = 0; i < NM; i++)
		{
			std::cout << "(" << matches[i].queryIdx << ", " << matches[i].trainIdx << ") ";
			if ((i+1) % 10 == 0)
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl << "]" << std::endl;
	}

};


/* 
*	Struct : Constraints
*	Description : Information on constaints
*
*	int NP : number of good pairs	
*	int NT : number of good transform pairs
*	std::vector<DMatch> matches : all the matches
*	std::vector<SFeatures> features : all the sift
*/
struct Constraints
{
	int NP;
	int NT;

	std::vector<struct Matchespp> matches;
	std::vector<struct SFeatures> features;

	Constraints()
	{
		NP = 0;
	}

	void reset()
	{
		NP = 0;
		matches.clear();
		features.clear();
	}

	void assignMatches(const Constraints &new_container)
	{
		NP = new_container.NP;
		matches = new_container.matches;
	}

	int getIdx(int i, int j)
	{
		int n = matches.size();

		for(int k = 0; k < n; k++)
			if (matches[k].idx[0] == i && matches[k].idx[1] == j)
				return k;

		return -1;
	}

	void print()
	{
		std::cout << std::endl;
		for(int i = 0; i < NP; i++)
			matches[i].print();
		std::cout << std::endl;
	}

	void printFeatures(int n)
	{
		int size = features.size();
		for (int i = 0; i < size; i++)
		{
			features[i].print(F_KEYS, n);
		}
	}

};



void readAndAdjustSiftFile(const std::string &path, const std::string &img, const std::string &file, struct SFeatures &container);
void readKeys(const util::Directory &dir, struct Constraints &container);
void readMatchFile(const std::string &path, struct Constraints &container);
void pruneDoubleMatch(struct Constraints &container);
void pruneDoubleMatch(struct Matchespp &new_box, const struct Matchespp &box);
float distancePL(float point[2], float line[3]);
void fMatrixFilter(struct Constraints &container);
int fMatrixFilter(const std::vector<cv::KeyPoint> &keys1, const std::vector<cv::KeyPoint> &keys2, std::vector<cv::DMatch> &list, float treshold = 3.0);
void transformInfo(struct Constraints &container);
void transformInfo(const std::vector<cv::KeyPoint> &keys1, const std::vector<cv::KeyPoint> &keys2, struct Matchespp &list, float treshold = 6.0);
void writeConstraints(const std::string &path, const std::vector<struct Matchespp> &container, int NP, int NT);
void geometry1Core(const util::Directory &dir);
void geometryMCore(const std::string &path, int numcore);
void geometryMCCore(const std::string &path, int numcore);


#endif