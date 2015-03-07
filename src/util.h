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

/*
*	Class : Distribution
*	Description : Class used to create distributions patterns
*
*	Methods :
*
*		Distribution(const util::Directory &dir, int netSize, DistType m_type) : Default constructor
*   Distributipn(const Distribution &p_dist) : Copy constructor
*   ~Distribution(); Default Destructor
*
*	Attributes :
*
*   DistType m_type : distribution type
*   int size : size of the distribution table
*		int* m_dist : distribution pattern
*
*/

class Distribution
{

public:

  Distribution(const util::Directory &dir, int netSize, DistType m_type);
  Distributipn(const Distribution &p_dist);
  Distribution()

private:

	DistType m_type;
	int m_size;
	int *m_dist;

};

//other util functions
void showProgress(int i, int n, int w, int actualize);
int ffind(FILE* f, const std::string &sr, char* buffer);

#endif
