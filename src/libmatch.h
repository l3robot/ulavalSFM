/*
*	File : libmatch.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2015, March 7th
*	Version : 1.0
*
*	Description : Functions relative to match
*/

#ifndef DOMATCH
#define DOMATCH

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "libsift.h"
#include "libmatch.h"
#include "directory.h"
#include "libgeometry.h"

#define GEOFILE "ulavalSFM.txt"

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
        NI = 0;
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

	Matchespp(int i, int j)
	{
		idx[0] = i;
		idx[1] = j;
		NM = 0;
		H = cv::Mat(3, 3, CV_32F);
		ratio = 0.0;
	}

	Matchespp(const Matchespp &new_matches) : matches(new_matches.matches)
	{
		idx[0] = new_matches.idx[0];
		idx[1] = new_matches.idx[1];
		NM = new_matches.NM;
		H = new_matches.H;
		ratio = new_matches.ratio;
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

struct mArgs
{
	int verbose;
	int geometry;
	std::string workingDir;
	std::string siftDir;
	std::string matchFile;

	mArgs() {
		verbose = 0;
		geometry = 0;
	}

};

void mParseArgs(int argc, char *argv[], struct mArgs *args);
void mUsage(char *progName);

void listDir(const util::Directory &dir, std::vector<std::string> &list);
void readSiftFile(const std::string &path, const std::string &img, const std::string &file, struct SFeatures &container);
int doMatch(const struct SFeatures &img1, const struct SFeatures &img2, struct Matchespp &container, int geo = 1, float ratio = 0.6);

void write2File(int netID, std::string matchFile, const Matchespp &container, int geo);
void worker(const util::Directory &dir, int aim, int end, struct mArgs args, int netID, int netSize);

#endif
