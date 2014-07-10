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
#include <string>
#include <iostream>
#include <dirent.h>

#include "util.h"
#include "directory.h"

using namespace std;


///////////////////////////////////////////////////////////////
////                  PARSING FUNCTIONS                    ////
///////////////////////////////////////////////////////////////

/* 
*	Function : parseArgs
*	Description : Parse command line arguments
*	
*	argc : number of arguments
*	dir : arguments
*	o : configuration structure
*/
void parseArgs(int argc, char* argv[], Opts &o)
{
	int tamp = 0;

	for(int i = 1; i < argc; i++)
	{
		if ((argv[i])[0] == '-')
		{
			switch ((argv[i])[1])
				{
					case 'h':
					i = argc;
					o.choice = o.choice & 1;
					o.choice = o.choice | 1;
					break;
					case 'v':
					i = argc;
					o.choice = o.choice & 2;
					o.choice = o.choice | 2;
					break;
					case 'l':
					o.choice = o.choice & 4;
					o.choice = o.choice | 4;
					i++;
					if(i < argc)
					{
						o.dir.assign(argv[i]);
					}
					else
					{
						o.choice = o.choice & 0;
					}
					i = argc;
					break;
					case 'c':
					i++;
					if(i < argc)
					{
						if(!sscanf(argv[i], "%d", &tamp))o.cluster = false;
						if(tamp)o.cluster = true;
					}
					else
					{
						i = argc;
						o.choice = o.choice & 0;
					}
					break;
					case 'n':
					i++;
					if(i < argc)
					{
						if(!sscanf(argv[i], "%d", &o.cores))o.cores = 1;
						if(o.cores < 1)o.cores = 1;
					}
					else
					{
						i = argc;
						o.choice = o.choice & 0;
					}
					break;
					case 's':
					o.choice = o.choice | 8;
					i++;
					if(i < argc)
					{
						o.dir.assign(argv[i]);
					}
					else
					{
						i = argc;
						o.choice = o.choice & 0;
					}
					break;
					case 'm':
					o.choice = o.choice | 16;
					i++;
					if(i < argc)
					{
						o.dir.assign(argv[i]);
					}
					else
					{
						i = argc;
						o.choice = o.choice & 0;
					}
					break;
					case 'a':
					o.choice = o.choice | 32;
					i++;
					if(i < argc)
					{
						o.dir.assign(argv[i]);
					}
					else
					{
						i = argc;
						o.choice = o.choice & 0;
					}
					break;
					default:
					o.choice = o.choice & 0;
					break;
				}
		}
	}
}

/* 
*	Function : printVersion
*	Description : Print the software version
*/
void printVersion()
{
	cout << endl;
	cout << "-----------------------" << endl;
	cout << "|| ulavalSFM Manager ||" << endl;
	cout << "-----------------------" << endl << endl;

	cout << "Version : " << VERSION <<  endl;
	cout << "Author      : Émile Robitaille @ LERobot" << endl;
	cout << "Description : Manager used to run sift, match, structure from motion (not yet)" << endl << endl;
	cout << "License     : - This software is freeware, OpenSource, free to distribute" << endl;
	cout << "              - Extern libraries/softwares keep of course their license(s)" << endl << endl;
}

/* 
*	Function : printHelp
*	Description : Print the help menu
*/
void printHelp()
{
	cout << endl;
	cout << "-----------------------" << endl;
	cout << "|| ulavalSFM Manager ||" << endl;
	cout << "-----------------------" << endl << endl;

	cout << "Version : " << VERSION <<  endl;
	cout << "Author : Émile Robitaille @ LERobot" << endl << endl;

	cout << "-------------- HELP MENU ----------------" << endl << endl;
	cout << "-h  ---      : Print this menu" << endl;
	cout << "-v  ---      : Print the software version" << endl;
	cout << "-l [dir]     : Print information about the directory" << endl;
	cout << "-c [0-1]     : On cluster or not. If 1, a script .sh file will be generated (default 0)" << endl;
	cout << "-n [1-*]     : Specify the number of core(s) wanted (default 1, means no mpi)" << endl;
	cout << "-s [dir]     : To find sift features of the directory images" << endl;
	cout << "-m [dir]     : To match sift features of the directory images" << endl;
	cout << "-a [dir]     : Do \"-s dir\" and then \"-m dir\"" << endl << endl;
	cout << "Refer to README file for more information about the version" << endl << endl;
}

/* 
*	Function : printError
*	Description : Print the parse error
*/
void printError()
{
	cout << endl;
	cout << "[ERROR] Empty or bad option(s), check the help menu below" << endl << endl;
	printHelp();
}

/* 
*	Function : listDir
*	Description : Give information on the directory
*
*	dir : working directory
*/
void listDir(const util::Directory &dir)
{
	dir.printInfo();
}

///////////////////////////////////////////////////////////////
////              MPI DISTRIBUTION FUNCTIONS               ////
///////////////////////////////////////////////////////////////


/* 
*	Function : createDist
*	Description : create a distribution for the mpi workers
*
*	dir : working directory
*	netSize : size of network
*/
int* createDist(const util::Directory &dir, int netSize)
{
	int* dist = (int*) malloc(2 * netSize * sizeof(int));

	int factor = dir.getNBImages() / netSize;
	int error = dir.getNBImages() % netSize;

	dist[0] = 0;

	if (error > 0)
	{
		dist[1] = factor + 1;
	}
	else
	{
		dist[1] = factor;
	}

	for(int i = 2; i < 2 * netSize; i+=2)
	{
		dist[i] = dist[i - 1];
		
		if (i / 2 < error)
		{
			dist[i + 1] = dist[i] + factor + 1;
		}
		else
		{
			dist[i + 1] = dist[i] + factor;
		}
	}

	return dist;
}


/* 
*	Function : deleteDist
*	Description : delete a distribution
*
*	dist : the distribution to delete
*/
void deleteDist(int* dist)
{
	free(dist);
}

int* createDist4Match(int numimages, int numcore)
{
	int numtask = numimages * (numimages - 1) / 2;

	int* dist = (int*) malloc(2 * numcore * sizeof(int));

	int distFactor = numtask / (numcore - 1); 
	int distError = numtask % (numcore - 1);

	dist[0] = -1;
	dist[1] = 0;

	for (int i = 2; i < 2 * numcore; i+=2)
	{
		dist[i] = dist[i - 1];
		if (i / 2 <= distError)
		{
			dist[i + 1] = dist[i] + distFactor + 1;
		}
		else
		{
			dist[i + 1] = dist[i] + distFactor;
		}
	}

	return dist;
}



























