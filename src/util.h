/*
*	File : util.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*	
*	Description : Bunch of useful functions
*/

#ifndef UTIL
#define UTIL

#include "directory.h"

//MPI distribution functions
int* createDist(const util::Directory &dir, int netSize);
void deleteDist(int* dist);
int* createDist4Match(int numimages, int numcore);
int* createDist4Geometry(int NP, int netSize);

//other util functions
void showProgress(int i, int n, int w, int actualize);
int ffind(FILE* f, const std::string &sr, char* buffer);
void createSubmit(const std::string &path, int numcore, int seconds, int option);

//Bundler functions
void createOptions(const std::string &path);
void Bundler(const std::string &path);

#endif
