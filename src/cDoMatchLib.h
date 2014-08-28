/*
*	File : cDoMatchLib.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/30/2014
*	Version : 1.0
*	
*	Description : Functions relative to match in parallel
*/

#ifndef CDOMATCH
#define CDOMATCH

#include <vector>
#include <string>

#include "dosift.h"
#include "domatch.h"
#include "directory.h"
#include "dogeometry.h"

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












