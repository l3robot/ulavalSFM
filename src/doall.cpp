/*
*	File : doall.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/30/2014
*	Version : 1.0
*	
*	Description : Functions relative to all
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

#include "util.h"
#include "doall.h"
#include "domatch.h"
#include "dosift.h"
#include "directory.h"

using namespace std;
using namespace cv;

/* 
*	Function : all1Core
*	Description : do all the algorithms on 1 core
*	
*	dir : directory information
*/
void all1Core(const util::Directory &dir)
{
	sift1Core(dir);
	match1Core(dir);
	Bundler(dir.getPath());
}

/* 
*	Function : allMCore
*	Description : do all the algorithms on the given number of cores (bundler on 1 core)
*	
*	path : working directory
*	numcore : number of cores
*/
void allMCore(const string &path, int numcore)
{
	stringstream c;

	c << "mpirun -n " << numcore << " cDoAll " << path;

	string command = c.str();

	system(command.c_str());
}

/* 
*	Function : allCMCore
*	Description : do all the algorithms on the given number of cores and on the supercomputer (bundler on 1 core)
*	
*	path : working directory
*	numcore : number of cores
*/
void allMCCore(const string &path, int numcore, int seconds)
{
	stringstream c;

	system("mkdir ulavalSub");

	printf("--> Create the script : \n");

	createSubmit(path, numcore, seconds);

	printf("--> Launch the script : \n");

	c << "msub ulavalSub/submit.sh";

	string command = c.str();

	system(command.c_str());

	cout << "Process launch, you can enter the command \"watch -n 10 showq -u $USER\" to see the progression." << endl << endl;
}












