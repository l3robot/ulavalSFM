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
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <string>

#include "directory.h"

#define F_NO_N -1
#define F_KEYS 1
#define F_DES 2
#define F_ALL 3

export char verbose = 0;
export char *siftPath = NULL;


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


void doSift(const std::string &path, struct SFeatures &container);
void writeSiftFile(const std::string &file, const struct SFeatures &container);


#endif
