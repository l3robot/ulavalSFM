/*
*	File : libmatch.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2015, March 7th
*	Version : 1.0
*
*	Description : Functions relative to match
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <queue>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <mpi.h>
#if CV_VERSION_MAJOR == 2
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#elif CV_VERSION_MAJOR == 3
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#endif

#include "directory.h"
#include "util.h"
#include "libsift.h"
#include "libmatch.h"
#include "libgeometry.h"

using namespace std;
using namespace cv;
#if CV_VERSION_MATJOR == 3
using namespace xfeatures2d;
#endif


/*
*	Function : sParseArgs
*	Description : Parse the arguments for the matching sift program
*
*	argc : argc main argument
*	argv : argv main argument
*	args : a structure with option information
*/
void sParseArgs(int argc, char *argv[], struct sArgs *args)
{
  //Check the number of arguments
	if (argc < 2)
		sUsage(argv[0]);

  char c;

  string Dir(argv[argc-1]);
	string File(argv[argc-1]);

	if (Dir[Dir.size()-1] != '/')
		Dir.append("/");

	if (File[File.size()-1] != '/')
		File.append("/");

  args->workingDir.assign(Dir);

  Dir.append("ulsift/");
	File.append("matches.init.txt");

  while ((c = getopt(argc, argv, "vgs:o:")) != -1)
  {
    switch(c)
    {
      case 'v':
        args->verbose = 1;
        break;

			case 'g':
	      args->verbose = 1;
	      break;

      case 's':
        args->siftDir.assign(optarg);
        break;

			case 'o':
	      args->matchFile.assign(optarg);
	      break;

      case '?' :
        sUsage(argv[0]);
        break;

      default:
        printf(" <-- Error while parsing -%c, read usage below\n", c);
        sUsage(argv[0]);
    }
  }

  if (args->siftDir.empty())
    args->siftDir.assign(Dir);
	if (args->matchFile.empty())
		args->matchFile.assign(File);
}

/*
*	Function : sUsage
*	Description : Show usage for the matching sift program
*
* progName : name of the program
*
*/
void sUsage(char *progName)
{
  printf("This is ulmatch (ulavalSFM match). Use it to match the sift points you found.\n");
  printf("Louis-Émile Robitaille @ L3Robot\n");
  printf("usage: mpirun -n [numberOfCores] %s [-vg] [-s Path] [-o Path] [workingDirectory]\n", progName);
  printf("      -v verbose mode, print a progress bar\n");
	printf("      -g geometry mode, do some geometric computations\n");
  printf("      -s [siftPath] set the sift directory path\n");
	printf("      -o [matchFilePath] set the match file path\n");
  exit(1);
}


/*
*	Function : listDir
*	Description : to make a list of .key file path
*
*	dir : directory information
*	list : vector of path which will be created
*/
void listDir(const util::Directory &dir, vector<string> &list)
{
	string path(dir.getPath());

	int numimages = dir.getNBImages();

	for(int i = 0; i < numimages; i++)
	{
		path.append(dir.getImage(i));
		while(path[path.size() - 1] != '.') path.pop_back();
		path.append("key");
		list.push_back(path);
		while(path[path.size() - 1] != '/') path.pop_back();
	}
}


/*
*	Function : doMatch
*	Description : match an image pair
*
*	img1 : container with img1 sift information
*	img2 : container with img2 sift information
*	container : matches information
*	geo : if to do geometry or not
*	container : ratio for the distance filter
*
*	return : 1 if NI > 0, else 0
*/
int doMatch(const SFeatures &img1, const SFeatures &img2, Matchespp &container, int geo, float ratio)
{
    #if CV_VERSION_MAJOR == 3
	FlannBasedMatcher matcher(makePtr<flann::IndexParams>(), makePtr<flann::SearchParams>(64));
    #else
	FlannBasedMatcher matcher(new flann::KDTreeIndexParams(16), new flann::SearchParams(64));
    #endif
	vector<vector<DMatch> > two_matches;

	matcher.knnMatch(img2.des, img1.des, two_matches, 2);

	for(int i = 0; i < img2.des.rows; i++)
	{
		if(two_matches[i][0].distance < ratio * two_matches[i][1].distance)
		{
			container.NM++;
			container.matches.push_back(two_matches[i][0]);
		}
	}

	//Prune double matches
	struct Matchespp new_container(container.idx);

	pruneDoubleMatch(new_container, container);

	if (new_container.NM >= 20) container.assign(new_container);
	else container.reset();
	///////////////////////

	//FMatrix filter
	if (container.NM > 0)
	{
		int NI = fMatrixFilter(img2.keys, img1.keys, container.matches);

		//cout << "FMATRIX : [ " << container.idx[0] << ", " << container.idx[1] << " ] : " << NI << " inliers found out of " << container.NM << endl;

		if (NI >= 16) container.NM = NI;
		else container.reset();
	}
	////////////////

	if (geo)
	{
		//Transform info
		if (container.NM > 0)
		{
			transformInfo(img2.keys, img1.keys, container);

			//cout << "TRANSFORM : [ " << container.idx[0] << ", " << container.idx[1] << " ] : " << container.NI << " inliers found out of " << container.NM << endl;

			if (container.NI < 10){ container.reset(false); return 0;}

			return 1;
		}
		////////////////

	}

	return 0;
}


/*
*	Function : serializeVector
*	Description : to serialize a vector
*
*	sender : vector to serialize
*	img1 : images 1 index
*	img2 : images 2 index
*/
void serializeContainer(const struct Matchespp &container, float *mBuffer, int mS, float *gBuffer, int gS)
{
	float* serialTab;

	int s = 4 + container.NM * 2;

	serialTab = (float*) malloc(s * sizeof(float));

	mBuffer[0] = (float) s;
	mBuffer[1] = (float) container.idx[0];
	mBuffer[2] = (float) container.idx[1];
	mBuffer[3] = (float) container.NM;

	for(int i = 0; i < container.NM * 2; i+=2)
	{
		int idx = i + 4;
		int idx2 = i/2;
		mBuffer[idx] = (float) container.matches[idx2].queryIdx;
		mBuffer[idx+1] = (float) container.matches[idx2].trainIdx;
	}

	gBuffer[0] = (float) container.NI;

	const double* M = container.H.ptr<double>();

	gBuffer[1] = (float) M[0];
	gBuffer[2] = (float) M[1];
	gBuffer[3] = (float) M[2];
	gBuffer[4] = (float) M[3];
	gBuffer[5] = (float) M[4];
	gBuffer[6] = (float) M[5];
	gBuffer[7] = (float) M[6];
	gBuffer[8] = (float) M[7];
	gBuffer[9] = (float) M[8];

	gBuffer[10] = container.ratio;
}



/*
*	Function : worker
*	Description : code for the workers
*
*	dir : directory information
*	recv : relative information about distribution
*	geo : if to do geometry or not
*/
void worker(const util::Directory &dir, int aim, int end, int geo, MPI_File file)
{
	int netID, seek = 0, compute = 0, stop = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	vector<string> list;

	listDir(dir, list);

	struct SFeatures keys1, keys2;

	float* serialMatch;

	queue<float*> master;

	for(int i = 0; !stop; i++)
	{
		if(compute)
		{
			readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), list[i], keys1);
		}

		for (int j = 0; j < i; j++)
		{

			if(seek == aim){readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), list[i], keys1);compute=1;}

			if(compute)
			{
				struct Matchespp container(j, i);

				readAndAdjustSiftFile(dir.getPath(), dir.getImage(j), list[j], keys2);

				doMatch(keys1, keys2, container, geo);

				float *mbuffer, *gbuffer;

				serialMatch = serializeContainer(container, mbuffer, gbuffer); // CHANGER, pas d'allocation dans une fonction!!

				container.reset();

				write2File(file, master, buffer);

				free(gbuffer)
			}

			seek++;
			if(seek == end){compute=0;stop=1;}
		}
	}

	while (!master.empty())
		write2File(file, master, NULL);
}

void write2File(MPI_File file, queue<float*> master, float* buffer)
{}
