/*
*	File : doall.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/30/2014
*	Version : 1.0
*	
*	Description : Functions relative to all
*/

#ifndef DOALL
#define DOALL

#include <string>

#include "directory.h"

void all1Core(const util::Directory &dir);
void allMCore(const std::string &path, int numcore);
void allMCCore(const std::string &path, int numcore, int seconds);

#endif












