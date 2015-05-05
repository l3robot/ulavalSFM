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
#include <mpi.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
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

/* CV_LOAD_IMAGE_GRAYSCALE is renamed to IMREAD_GRAYSCALE in OpenCV 3 */
#if CV_VERSION_MAJOR == 3
    using namespace xfeatures2d;

    #define CV_LOAD_IMAGE_GRAYSCALE IMREAD_GRAYSCALE
    #include <opencv2/calib3d/calib3d_c.h>
#endif


/*
*	Function : sParseArgs
*	Description : Parse the arguments for the matching sift program
*
*	argc : argc main argument
*	argv : argv main argument
*	args : a structure with option information
*/
void mParseArgs(int argc, char *argv[], struct mArgs *args)
{
  //Check the number of arguments
	if (argc < 2)
		mUsage(argv[0]);

  char c;

  string Dir(argv[argc-1]);
	string File(argv[argc-1]);
	string gFile(argv[argc-1]);

	if (Dir[Dir.size()-1] != '/')
		Dir.append("/");

	if (File[File.size()-1] != '/')
		File.append("/");

	if (gFile[gFile.size()-1] != '/')
		gFile.append("/");

  args->workingDir.assign(Dir);

  Dir.append("ulsift/");
	File.append("matches.init.txt");
	gFile.append("ulavalSFM.txt");

  while ((c = getopt(argc, argv, "vgs:o:f:")) != -1)
  {
    switch(c)
    {
      case 'v':
        args->verbose = 1;
        break;

			case 'g':
	      args->geometry = 1;
	      break;

      case 's':
        args->siftDir.assign(optarg);
        if (args->siftDir[args->siftDir.size-1] != '/')
          args->siftDir.append('/');
        break;

			case 'o':
	      args->matchFile.assign(optarg);
	      break;

			case 'f':
		    args->geoFile.assign(optarg);
		    break;

      case '?' :
        mUsage(argv[0]);
        break;

      default:
        printf(" <-- Error while parsing -%c, read usage below\n", c);
        mUsage(argv[0]);
    }
  }

  if (args->siftDir.empty())
    args->siftDir.assign(Dir);
	if (args->matchFile.empty())
		args->matchFile.assign(File);
	if (args->geoFile.empty())
		args->geoFile.assign(gFile);
}

/*
*	Function : sUsage
*	Description : Show usage for the matching sift program
*
* progName : name of the program
*
*/
void mUsage(char *progName)
{
  printf("This is ulmatch (ulavalSFM match). Use it to match the sift points you found.\n");
  printf("Louis-Émile Robitaille @ L3Robot\n");
  printf("usage: mpirun -n [numberOfCores] %s [-vg] [-s Path] [-o Path] [-f Path] [workingDirectory]\n", progName);
  printf("      -v verbose mode, print a progress bar (default false)\n");
	printf("      -g geometry mode, do some geometric computations (default false)\n");
  printf("      -s [siftPath] set the sift directory path (default ulsift/)\n");
	printf("      -o [matchFilePath] set the match file path (default matches.init.txt)\n");
	printf("      -f [geoFilePath] set the geometric file path (default ulavalSFM.txt)\n");
  exit(1);
}


/*
*	Function : listDir
*	Description : to make a list of .key file path
*
*	dir : directory information
*	list : vector of path which will be created
*/
void listDir(const util::Directory &dir, const string siftDir,vector<string> &list)
{
	string path(siftDir);

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
*	Function : readSiftFile
*	Description : Read the sift file, adjust the coordinates and fill a SFeatures object
*
*	file : path of the .key file
*	container : container for sift keypoints and their descriptor
*/
void readSiftFile(const string &path, const string &img, const string &file, struct SFeatures &container)
{
	Mat buffer;

	container.keys.clear();

	string true_img = path;
	true_img.append(img);

	buffer = imread(true_img.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

	float height = (float) buffer.rows;
	float width = (float) buffer.cols;

	FILE* f = fopen(file.c_str(), "r");

	if(!f)
	{
		cout << "[ERROR] The file " << file << " does not exist" << endl;
		cout << "[ERROR] Maybe you haven't found sift point yet" << endl;
		cout << "[ERROR] Program is forced to quit" << endl;
		exit(1);
	}

	float garbage, *pter;

	fscanf(f, "%d %f", &container.NF, &garbage);

	Mat descriptor(container.NF, 128, CV_32F);
	KeyPoint key;

	for(int i = 0; i < container.NF; i++)
	{
		fscanf(f, "%f %f %f %f", &key.pt.y, &key.pt.x, &garbage, &garbage);

		//reverse y axis
		key.pt.y = height - key.pt.y - 1.0;

		//put the image center at (0, 0)
		key.pt.x -= 0.5 * (width - 1);
        key.pt.y -= 0.5 * (height - 1);

		container.keys.push_back(key);

		pter = descriptor.ptr<float>(i);

		for(int j = 0; j < 128; j++)
		{
			fscanf(f, "%f", &pter[j]);
		}
	}

	container.des = descriptor;

	fclose(f);
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
	FlannBasedMatcher matcher(makePtr<flann::KDTreeIndexParams>(), makePtr<flann::SearchParams>(64));
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
  if (geo) {
	struct Matchespp new_container(container.idx);

	pruneDoubleMatch(new_container, container);

	if (new_container.NM >= 20) container.assign(new_container);
	else container.reset();}
	///////////////////////

	//FMatrix filter
	if (geo && container.NM > 0)
	{
		int NI = fMatrixFilter(img2.keys, img1.keys, container.matches);

		if (NI >= 16) container.NM = NI;
		else container.reset();
	}
	////////////////

	if (geo && container.NM > 0)
	{
		//Transform info
		transformInfo(img2.keys, img1.keys, container);

		if (container.NI < 10){ container.reset(false); return 0;}

		return 1;
	}

	return 0;
}


/*
*	Function : write2File
*	Description : write match information between 2 images in a file
*
*	netID : ID of the core
*	matchFile : where to right the information
*	container : matches information
*	geo : if to do geometry or not
*
*/
void write2File(int netID, string matchFile, string geoFile, const vector<struct Matchespp> &master, int geo)
{
	FILE *f1, *f2;

	if (netID == 0) {
		f1 = fopen(matchFile.c_str(), "wb");
		if (geo)
			f2 = fopen(geoFile.c_str(), "wb");
	}
	else {
		f1 = fopen(matchFile.c_str(), "ab");
		if (geo)
			f2 = fopen(geoFile.c_str(), "ab");
	}

	for (int i = 0; i < master.size(); i++) {
		fprintf(f1, "%d %d\n", master[i].idx[0], master[i].idx[1]);
		fprintf(f1, "%d\n", master[i].NM);

		for(int j = 0; j < master[i].NM; j++)
			fprintf(f1, "%d %d\n", master[i].matches[j].queryIdx, master[i].matches[j].trainIdx);

		if(master[i].NI > 0 && geo)
		{
			fprintf(f2, "%d %d\n", master[i].idx[0], master[i].idx[1]);

			fprintf(f2, "%d\n", master[i].NI);
			fprintf(f2, "%f\n", master[i].ratio);

			const double* M = master[i].H.ptr<double>();

			fprintf(f2, "%f %f %f %f %f %f %f %f %f\n", (float) M[0], (float) M[1],
			(float) M[2], (float) M[3], (float) M[4], (float) M[5],
			(float) M[6], (float) M[7], (float) M[8]);
		}
	}

	fclose(f1);

	if (geo)
		fclose(f2);
}


/*
*	Function : worker
*	Description : code for the workers
*
*	dir : directory information
*	recv : relative information about distribution
*	geo : if to do geometry or not
*/
void worker(const util::Directory &dir, int aim, int end, struct mArgs args, int netID, int netSize)
{
	int seek = 0, compute = 0, stop = 0, control = 0;
	int total = end - aim;

	MPI_Status status;

	vector<string> list;

	listDir(dir, args.siftDir, list);

	struct SFeatures keys1, keys2;

	vector<Matchespp> master;

	if (netID == 0 && args.verbose)
		showProgress(0, total, 75, 1);

	for(int i = 0; !stop; i++)
	{
		if(compute)
		{
			readSiftFile(dir.getPath(), dir.getImage(i), list[i], keys1);
		}

		for (int j = 0; j < i; j++)
		{

			if(seek == aim){readSiftFile(dir.getPath(), dir.getImage(i), list[i], keys1);compute=1;}

			if(compute)
			{
				struct Matchespp container(j, i);

				readSiftFile(dir.getPath(), dir.getImage(j), list[j], keys2);

				doMatch(keys1, keys2, container, args.geometry);

				master.push_back(container);

				if (netID == 0 && args.verbose)
					showProgress(seek-aim, total, 75, 1);
			}

			seek++;
			if(seek == end){compute=0;stop=1;}
		}
	}

	if (netID == 0 && args.verbose)
		showProgress(total, total, 75, 0);

	if (netID > 0)
		MPI_Recv(&control, 1, MPI_INT, netID-1, 1, MPI_COMM_WORLD, &status);

	write2File(netID, args.matchFile, args.geoFile, master, args.geometry);

	if (netID < netSize-1)
		MPI_Send(&control, 1, MPI_INT, netID+1, 1, MPI_COMM_WORLD);
}
