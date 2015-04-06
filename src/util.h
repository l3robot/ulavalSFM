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

typedef enum {
  DIST4SIFT=0,
  DIST4MATCHES
} DistType;

void distribution(int id, int size, util::Directory &dir, DistType type, int *start, int *end);

//other util functions
void showProgress(int i, int n, int w, int actualize);
int ffind(FILE* f, const std::string &sr, char* buffer);

#endif
