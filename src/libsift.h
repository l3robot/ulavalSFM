/*
*	File : libsift.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2015, March 7th
*	Version : 1.0
*
*	Description : Functions relative to sift
*/

#ifndef LIBSIFT
#define LIBSIFT

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#if CV_VERSION_MAJOR == 2
#include <opencv2/features2d/features2d.hpp>
#elif CV_VERSION_MAJOR == 3
#include <opencv2/xfeatures2d.hpp>
#endif
#include <vector>
#include <string>

#include "directory.h"

#define F_NO_N -1
#define F_KEYS 1
#define F_DES 2
#define F_ALL 3


/*
*	Struct : SFeatures
*	Description : Information on sift features
*
*	int NF : number of features
*	std::vector<cv::KeyPoint> keys : keypoints carateristics
*	cv::Mat des : keypoints descriptor
*/
struct SFeatures
{
	int NF;

	std::vector<cv::KeyPoint> keys;
	cv::Mat des;

	SFeatures()
	{
		NF = 0;
	}

	void reset()
	{
		NF = 0;
		keys.clear();
	}

};

struct sArgs
{
	int verbose;
	std::string workingDir;
	std::string siftDir;

	sArgs() {
		verbose = 0;
	}
};

void sParseArgs(int argc, char *argv[], struct sArgs *args);
void sUsage(char *progName);

void doSift(const std::string &path, struct SFeatures &container);
void writeSiftFile(const std::string &file, const struct SFeatures &container);


#endif
