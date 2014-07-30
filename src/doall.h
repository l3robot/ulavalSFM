/*
*	File : domatch.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
*	Version : 1.0
*	
*	Description : Functions relative to match
*/

#ifndef DOALL
#define DOALL

#include <vector>
#include <string>

#include "util.h"
#include "directory.h"

all1Core(const util::Directory &dir);
allMCore(const std::string &path, int numcore);
allMCcore(const std::string &path, int numcore, int seconds);

#endif












