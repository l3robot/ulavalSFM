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

	void print(int c, int n)
	{
		if (n < 0)
		{
			if (c & 1)
			{
				std::cout << std::endl;
				std::cout << "Header : " << std::endl;
				int size = keys.size();
				for (int i = 0; i < size; i++)
					std::cout << "( " << keys[i].pt.x << ", " << keys[i].pt.y << ")" << std::endl;
			}
			if (c & 2)
			{
				std::cout << std::endl;
				std::cout << "Descriptor : " << std::endl;
				std::cout << des << std::endl;	
			}
		}
		else
		{
			if (c & 1)
			{
				std::cout << std::endl;
				std::cout << "Header [" << n << "] :" << std::endl;
				std::cout << "( " << keys[n].pt.x << ", " << keys[n].pt.y << ")" << std::endl;
			}
		}
	}
}; 


void doSift(const std::string &path, struct SFeatures &container);
void writeSiftFile(const std::string &file, const struct SFeatures &container);
void sift1Core(const util::Directory &dir);
void siftMCore(const std::string &path, int numcore);
void siftMCCore(const std::string &path, int numcore, int seconds);







#endif
