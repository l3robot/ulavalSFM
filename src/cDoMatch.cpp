/*
*	File : cDoSift.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 07/03/2014
*	Version : 1.0
*	
*	Description : Program to make match in parallel
*/





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <time.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>

using namespace std;
using namespace cv;






int main(int argc, char** argv)
{
	char** list;
	int nb_lines;
	double time;

	Mat keys, keys2, pts1, pts2;

	vector<DMatch> matches;

	clock_t start = clock();

	FILE* f = fopen("matches.mat", "w");

	for(int path = 1; path < argc; path++)
	{
		list = list_dir(argv[path], &nb_lines);

		for(int i = 0; i < nb_lines; i++)
		{
			keys = readSiftFile(list[i], pts1);

			for(int j = i + 1; j < nb_lines; j++)
			{
				keys2 = readSiftFile(list[j], pts2);
				matches = doMatch(keys, keys2, pts1, pts2);
				writeMatch(f, matches, list[i], list[j]);
				printf("%08lu appariements trouvés entre l'image %s et l'image %s\n", matches.size(), list[i], list[j]);
			}
		}

		free(list);
	}

	time = (clock() - start) / (double) CLOCKS_PER_SEC;

	printf("Le programme a pris environ %f secondes\n", time);

	fclose(f);

	return 0;
}












