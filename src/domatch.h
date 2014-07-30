/*
*	File : domatch.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
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

#include "dosift.h"
#include "directory.h"

#define MATCHFILE "matches.init.txt"
#define BOSS 0
#define SECRETARY 0

/* 
*	Struct : Matches
*	Description : Information on matches
*
*	int NM : number of matches	
*	std::vector<DMatch> matches : matches
*/
struct Matches
{
	int NM;

	std::vector<cv::DMatch> matches;

	Matches()
	{
		NM = 0;
	}

	void reset()
	{
		NM = 0;
		matches.clear();
	}

};

void listDir(const util::Directory &dir, std::vector<std::string> &list);
void readSiftFile(const std::string &file, struct SFeatures &container);
int doMatch(const struct SFeatures &img1, const struct SFeatures &img2, struct Matchespp &container, float ratio = 0.6);
void writeMatchFile(FILE* f, const struct Matches &container, int i, int j);
void match1Core(const util::Directory &dir);
void matchMCore(const std::string &path, int numcore);
void matchMCCore(const std::string &path, int numcore, int seconds);

#endif












