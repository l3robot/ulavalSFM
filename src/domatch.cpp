/*
*	File : domatch.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
*	Version : 1.0
*	
*	Description : Functions relative to match
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "dosift.h"

Mat readSiftFile(const string &file, SFeatures &container)
{
	FILE* f = fopen(file.c_str(), "r");

	if(!f)
	{
		cout << "[ERROR] The file " << file << " does not exist" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	float garbage, pt, *pter;

	fscanf(f, "%d %d", &container.NF, &garbage);

	for(int i = 0; i < num; i++)
	{
		pter = t_pts.ptr<float>(i);

		fscanf(f, "%f %f %f %f", &pter[0], &pter[1], &garbage, &garbage);

		pter = descriptor.ptr<float>(i);

		for(int j = 0; j < 128; j++)
		{
			fscanf(f, "%f", &pt);
			pter[j] = pt;
		} 

	}
	fclose(f);

	pts = t_pts;
	return descriptor;
}
