/*
*	File : util.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*
*	Description : Bunch of util functions
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <dirent.h>

#include "util.h"
#include "directory.h"

using namespace std;

///////////////////////////////////////////////////////////////
////              MPI DISTRIBUTION FUNCTIONS               ////
///////////////////////////////////////////////////////////////


void distribution(int id, int size, util::Directory &dir, DistType type, int *start, int *end)
{
	int numimages = dir.getNBImages();

	if (type == DIST4MATCHES) {
		id--;
		size--;
		numimages = numimages * (numimages - 1) / 2;
	}

	int distFactor = numimages / size;
	int distError = numimages % size;

	if (id < distError) {
		*start = id * (distFactor + 1);
		*end = (id + 1) * (distFactor + 1);
	}
	else {
		*start = id * distFactor + distError;
		*end = *start + distFactor;
	}
}


///////////////////////////////////////////////////////////////
////                OTHER UTIL FUNCTIONS                   ////
///////////////////////////////////////////////////////////////




/*
*	Function : showProgress
*	Description : Print a progress bar to show progression
*
*	i : where you are
*	n : total of iterations
*	w : width of the bar
*	actualize : erase or not the bar
*/
void showProgress(int i, int n, int w, int actualize)
{
	float ratio = i / (float)n;
	int c = ratio * w;

	printf("%3d%% [", (int)(ratio*100) );

	for (int i = 0; i < c; i++)
		printf("=");

	for (int i = c; i < w; i++)
		printf(" ");
	if(actualize)
		printf("]\n\033[F\033[J");
	else
		printf("]\n");
}


/*
*	Function : ffind
*	Description : Find a option in an option file and store its value in buffers
*
*	f : file descriptor
*	sr : option to search
*	buffer : buffer to copy the option value
*
*	return : 1 if found, 0 if not
*/
int ffind(FILE* f, const string &sr, char* buffer)
{
	char* pter = NULL;

	while(fgets(buffer, 256, f))
	{
		if(strstr(buffer, sr.c_str()))
		{
			pter = strchr(buffer, ':');
			sprintf(buffer, "%s", pter+1);
			pter = strchr(buffer, ';');
			*pter = '\0';
			printf("Value %s found for option %s.\n", buffer, sr.c_str());
			return 1;
		}
	}

	return 0;
}
