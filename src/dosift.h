/*
*	File : dosift.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*	
*	Description : Functions relative to sift
*/

#ifndef DOSIFT
#define DOSIFT

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

#include "directory.h"


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
}; 


void doSift(const std::string &path, struct SFeatures &container);
void writeSiftFile(const std::string &file, const struct SFeatures &container);
void sift1Core(const util::Directory &dir);
void siftMCore(const std::string &path, int numcore);
void siftMCCore(const std::string &path, int numcore);







#endif
