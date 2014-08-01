/*
*	File : main.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*	
*	Description : Main of ulavalSFM
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>

#include "util.h"
#include "dosift.h"
#include "domatch.h"
#include "doall.h"

using namespace std;

int main(int argc, char* argv[])
{
	Opts o;

	//parsing arguments
	parseArgs(argc, argv, o);

	switch (o.choice)
	{
		case 0:
		printError();
		break;
		case 1:
		printHelp();
		break;
		case 2:
		printVersion();
		break;
		case 4:
		listDir(o.dir);
		break;
		case 8:
		cout << endl;
		cout << "Process(es) will do sift with " << o.cores << " core(s) on the " << BTOS(o.cluster) << ", working on \"" << o.dir.getPath() << "\" images." << endl << endl;
		if (o.cores > 1)
		{
			if (o.cluster)
			{
				siftMCCore(o.dir.getPath(), o.cores, o.seconds);
				o.dir.update();
			}
			else
			{
				siftMCore(o.dir.getPath(), o.cores);
				o.dir.update();
			}
		}
		else
		{
			if (o.cluster)
			{
				siftMCCore(o.dir.getPath(), o.cores, o.seconds);
				o.dir.update();
			}
			else
			{
				sift1Core(o.dir);
				o.dir.update();
			}
		}
		break;
		case 16:
		cout << endl;
		cout << "Process(es) will do matches with " << o.cores << " core(s) on the " << BTOS(o.cluster) << ", working on \"" << o.dir.getPath() << "\" images." << endl << endl;
		if (o.cores > 1)
		{
			if (o.cluster)
			{
				o.dir.createIMGlist();
				matchMCCore(o.dir.getPath(), o.cores, o.seconds);
				o.dir.update();
			}
			else
			{
				o.dir.createIMGlist();
				matchMCore(o.dir.getPath(), o.cores);
				o.dir.update();
			}
		}
		else
		{
			if (o.cluster)
			{
				o.dir.createIMGlist();
				matchMCCore(o.dir.getPath(), o.cores, o.seconds);
				o.dir.update();
			}
			else
			{
				o.dir.createIMGlist();
				match1Core(o.dir);
				o.dir.update();
			}
		}
		break;
		case 32:
		cout << endl;
		cout << "Process(es) will do bundlerSFM part working on \"" << o.dir.getPath() << "\" images." << endl << endl;
		Bundler(o.dir.getPath());
		break;
		case 64:
		cout << endl;
		cout << "Process(es) will do sift, matches and bundlerSFM with " << o.cores << " core(s) on the " << BTOS(o.cluster) << ", working on \"" << o.dir.getPath() << "\" images." << endl << endl;
		if (o.cores > 1)
		{
			if (o.cluster)
			{
				o.dir.createIMGlist();
				allMCCore(o.dir.getPath(), o.cores, o.seconds);
				o.dir.update();
			}
			else
			{
				o.dir.createIMGlist();
				allMCore(o.dir.getPath(), o.cores);
				o.dir.update();
			}
		}
		else
		{
			if (o.cluster)
			{
				o.dir.createIMGlist();
				allMCCore(o.dir.getPath(), o.cores, o.seconds);
				o.dir.update();
			}
			else
			{
				o.dir.createIMGlist();
				all1Core(o.dir);
				o.dir.update();
			}
		}
		break;
		default:
		printError();
		break;
	}
}
