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

struct sArgs
{
	int verbose;
	int geometry;
	std::string workingDir;
	std::string siftDir;
	std::string matchFile;

	sArgs() {
		verbose = 0;
		geometry = 0;
	}

};

void sParseArgs(int argc, char *argv[], struct sArgs *args);
void sUsage(char *progName);

void listDir(const util::Directory &dir, std::vector<std::string> &list);
void readSiftFile(const std::string &file, struct SFeatures &container);
int doMatch(const struct SFeatures &img1, const struct SFeatures &img2, struct Matchespp &container, int geo = 1, float ratio = 0.6);
int findIDX(int i, int j, const std::vector<struct Matchespp> &container, int* reverse);
void writeMatchFile(const std::string &path, const std::vector<struct Matchespp> &container, int n, int bar = 1);
void match1Core(const util::Directory &dir, int geo = 1);
void matchMCore(const std::string &path, int numcore, int geo);
void matchMCCore(const std::string &path, int numcore, int seconds);

int* boss(int numcore, const util::Directory &dir);
float* serializeContainer(const struct Matchespp &container);
void endComm(int sender);
void worker(const util::Directory &dir, int* recv, int geo = 1);
void writeSerialMatch(const std::string &path, const std::vector<float*> &container, int n, int bar);
float* searchIDX(int i, int j, const std::vector<float*> &container, int* reverse);
void writeSerialMatchespp(const std::string &path, const std::vector<float*> &container, int n, int bar);
float* recvFromWorker(std::vector<int> &list);
void secretary(const std::string &path, int numcore, int n, int bar, int geo = 1);

#endif
