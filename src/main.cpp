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
				cout << "MPI - ON SUPERCOMPTER" << endl << endl;
			}
			else
			{
				o.dir.printInfo();
				siftMCore(o.dir.getPath(), o.cores);
				o.dir.update();
				o.dir.printInfo();
			}
		}
		else
		{
			if (o.cluster)
			{
				cout << "NO MPI - ON SUPERCOMPTER" << endl << endl;
			}
			else
			{
				o.dir.printInfo();
				sift1Core(o.dir);
				o.dir.update();
				o.dir.printInfo();
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
				cout << "MPI - ON SUPERCOMPTER" << endl << endl;
			}
			else
			{
				cout << "MPI - ON COMPUTER" << endl << endl;
				/*
				o.dir.printInfo();
				siftMCore(o.dir.getPath(), o.cores);
				o.dir.update();
				o.dir.printInfo();
				*/
			}
		}
		else
		{
			if (o.cluster)
			{
				cout << "NO MPI - ON SUPERCOMPTER" << endl << endl;
			}
			else
			{
				cout << "NO MPI - ON COMPUTER" << endl << endl;
				/*
				o.dir.printInfo();
				sift1Core(o.dir);
				o.dir.update();
				o.dir.printInfo();
				*/
			}
		}
		break;
		case 32:
		cout << endl;
		cout << "Process(es) will do sift and matches with " << o.cores << " core(s) on the " << BTOS(o.cluster) << ", working on \"" << o.dir.getPath() << "\" images." << endl << endl;
		break;
		default:
		printError();
		break;
	}
}
