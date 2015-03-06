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

/* 
*	Function : createDist4Match
*	Description : create a distribution for the mpi workers
*
*	numimages : number of images
*	netSize : size of network
*/
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


/* 
*	Function : createDist4Geometry
*	Description : create a distribution for the mpi workers
*
*	NP : number of pairs
*	netSize : size of network
*/
int* createDist4Geometry(int NP, int netSize)
{
	int* dist = (int*) malloc(2 * netSize * sizeof(int));

	int factor = NP / (netSize - 1);
	int error = NP % (netSize - 1);

	dist[2] = 0;

	if (error > 0)
	{
		dist[3] = factor + 1;
	}
	else
	{
		dist[3] = factor;
	}

	for(int i = 4; i < 2 * netSize; i+=2)
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

/* 
*	Function : createSubmit
*	Description : Create the submit file for the supercomputer
*
*	path : working directory path
*	numcore : number of cores
*	seconds : walltime
*/
void createSubmit(const string &path, int numcore, int seconds, int option)
{
	system("mkdir ulavalSub");

	char buffer[256];

	strcpy(buffer, getenv("HOME"));

	string ulavalsfm(buffer);
	ulavalsfm.append("/.ulavalsfm");

	FILE* fr = fopen(ulavalsfm.c_str(), "r");

	if (!fr)
	{
		cout << "[ERROR] The file \"~/.ulavalsfm\" does not exist" << endl;
		cout << "[ERROR] Create a the file with the caracteristics you want" << endl;
		cout << "[ERROR] Use the README for more information" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	FILE* fw = fopen("ulavalSub/submit.sh", "w");

	if (!fw)
	{
		cout << "[ERROR] The file \"ulavalSubmit/submit.sh\" cannot be opened" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	if(!ffind(fr, "RAP", buffer))
	{
		cout << "[ERROR] You have not precised a RAP number in \"~/.ulavalsfm\"" << endl;
		cout << "[ERROR] Add the line : \"RAP:<RAP number>;\"" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	string RAP(buffer);

	fclose(fr);

	fprintf(fw, "%s\n", "# Shell used by ulavalsfm to launch matches phase -- ulavalsfm");
	fprintf(fw, "%s\n", "#PBS -S /bin/bash");
	fprintf(fw, "%s\n", "#PBS -N ulavalsfm_matches # Nom de la tâche");
	fprintf(fw, "%s%s%s\n", "#PBS -A ", RAP.c_str(), " # Identifiant Rap; ID");
	fprintf(fw, "%s%d%s\n", "#PBS -l nodes=", numcore/8, ":ppn=8 # Nombre de noeuds et nombre de processus par noeud");
	fprintf(fw, "%s%d%s\n", "#PBS -l walltime=", seconds, " # Durée en secondes");
	fprintf(fw, "%s\n", "#PBS -o ./ulavalSub/out.txt #sortie");
	fprintf(fw, "\n");
	fprintf(fw, "%s%s%s\n", "cd \"", path.c_str(), "\"");
	fprintf(fw, "\n");
	fprintf(fw, "%s\n", "module load compilers/gcc/4.8.0");
	fprintf(fw, "%s\n", "module load mpi/openmpi/1.6.4_gcc");
	fprintf(fw, "\n");
	if (option == 0)
		fprintf(fw, "%s\n", "mpiexec cDoSift . 0");
	else if (option == 1)
		fprintf(fw, "%s\n", "mpiexec cDoMatch . 0");
	else if (option == 2)
		fprintf(fw, "%s\n", "mpiexec cDoAll . 0");

	fclose(fw);
}



///////////////////////////////////////////////////////////////
////                  BUNDLER FUNCTIONS                    ////
///////////////////////////////////////////////////////////////


void createOptions(const string &path)
{
	string file(path);

	file.append("options.txt");

	FILE* f = fopen(file.c_str(), "w");

	fprintf(f, "--output_all bundle_\n");
	fprintf(f, "--constrain_focal\n");
	fprintf(f, "--estimate_distortion\n");
	fprintf(f, "--variable_focal_length\n");
	fprintf(f, "--output bundle.out\n");
	fprintf(f, "--output_dir bundle\n");
	fprintf(f, "--output_all bundle_\n");
	fprintf(f, "--use_focal_estimate\n");
	fprintf(f, "--match_table matches.init.txt\n");
	fprintf(f, "--run_bundle\n");
	fprintf(f, "--constrain_focal_weight 0.0001\n");

	fclose(f);
}

void Bundler(const string &path)
{
	createOptions(path);

	chdir(path.c_str());

	system("mkdir bundle");

	string command("bundler ");
	command.append("images.txt ");
	command.append("--options_file ");
	command.append("options.txt");

	system(command.c_str());
}













