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


Distribution::Distribution(DistType p_type, const util::Directory &dir, int netSize)
{
	int numimages = dir.getNBImages();
	m_size = 2 * netSize * sizeof(int);
	m_dist = (int*) malloc(m_size);

	switch (p_type) {

		case DIST4SIFT:

			m_type = DIST4SIFT;

			int distFactor = numimages / netSize;
			int distError = numimages % netSize;

			m_dist[0] = 0;

			if (distError > 0)
			{
				m_dist[1] = DistFactor + 1;
			}
			else
			{
				m_dist[1] = distFactor;
			}

			for(int i = 2; i < 2 * netSize; i+=2)
			{
				m_dist[i] = m_dist[i - 1];

				if (i / 2 < distError)
				{
					m_dist[i + 1] = m_dist[i] + distFactor + 1;
				}
				else
				{
					m_dist[i + 1] = m_dist[i] + distFactor;
				}
			}
			break;

		case DIST4MATCHES:

			m_type = DIST4SIFT;

			int numtask = numimages * (numimages - 1) / 2;

			int distFactor = numtask / (netSize - 1);
			int distError = numtask % (netSize - 1);

			m_dist[0] = -1;
			m_dist[1] = 0;

			for (int i = 2; i < 2 * netSize; i+=2)
			{
				m_dist[i] = m_dist[i - 1];
				if (i / 2 <= distError)
				{
					m_dist[i + 1] = m_dist[i] + distFactor + 1;
				}
				else
				{
					m_dist[i + 1] = m_dist[i] + distFactor;
				}
			}
			break;

		default:
			printf("Error in distribution creation, type does not exist !! Force to quit !!\n");
			exit(1);
			break;
	}
}

Distribution::Distribution(Distribution &dist)
{
	m_type = dist.m_type;
	free(m_dist);

	m_dist = malloc(dist.m_size);

	for(int i = 0; i < dist.m_size; i++)
		m_dist[i] = dist.m_dist[i];

}

Distribution::~Distribution()
{
	free(dist);
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
