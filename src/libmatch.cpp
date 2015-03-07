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
#include <vector>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
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
#include "dosift.h"
#include "domatch.h"
#include "dogeometry.h"
#include "cDoMatchLib.h"

using namespace std;
using namespace cv;
#if CV_VERSION_MATJOR == 3
using namespace xfeatures2d;
#endif


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
*	Function : readSiftFile
*	Description : read a .sift file
*
*	file : path of the .sift file
*	container : container for sift keypoints and their descriptor
*/
void readSiftFile(const string &file, SFeatures &container)
{
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

	if (geo)
	{

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
*	Function : findIDX
*	Description : find idx of a match
*
*	i : idx of image 1
*	j : idx of image 2
*	container : vector filled with all the matches
*	reverse : indicate if the images idx are in order or reversed
*
*	return : -1 if not found, idx if found
*/
int findIDX(int i, int j, const vector<struct Matchespp> &container, int* reverse)
{
	int num = container.size();

	*reverse = 0;

	for(int a = 0; a < num; a++)
	{
		if((int) container[a].idx[0] == i && (int) container[a].idx[1] == j)
			return a;
		if((int) container[a].idx[0] == j && (int) container[a].idx[1] == i)
		{
			*reverse = 1;
			return a;
		}
	}

	return -1;
}


/*
*	Function : writeMatchFile
*	Description : write matches in a file
*
*	path : path of the working directory
*	container : container with matches information
*	n : numbre of images
*	bar : print or not a progress bar
*/
void writeMatchFile(const string &path, const vector<struct Matchespp> &container, int n, int bar)
{
	string file(path);

	file.append("matches.init.txt");

	FILE* f = fopen(file.c_str(), "wb");

	int ni = ( 1 + sqrt( 1 + 8 * n ) ) / 2;

	for (int i = 0; i < ni; i++)
	{
		for (int j = 0; j < ni; j++)
		{
			int reverse;
			int idx = findIDX(i, j, container, &reverse);

			if (idx > 0 && !reverse)
			{
				struct Matchespp box(container[idx]);

				fprintf(f, "%d %d\n", box.idx[0], box.idx[1]);
				fprintf(f, "%d\n", box.NM);

				for(int j = 0; j < box.NM; j++)
					fprintf(f, "%d %d\n", box.matches[j].queryIdx, box.matches[j].trainIdx);
			}
			else if (idx > 0 && reverse)
			{
				struct Matchespp box(container[idx]);

				fprintf(f, "%d %d\n", box.idx[1], box.idx[0]);
				fprintf(f, "%d\n", box.NM);

				for(int j = 0; j < box.NM; j++)
					fprintf(f, "%d %d\n", box.matches[j].trainIdx, box.matches[j].queryIdx);
			}

			if (bar) showProgress(i * ni + j, ni * ni, 75, 1);

		}
	}

	if (bar) showProgress(ni * ni, ni * ni, 75, 0);

	fclose(f);
}


/*
*	Function : match1Core
*	Description : match points and write those match in files
*
*	dir : directory information
*/
void match1Core(const util::Directory &dir, int geo)
{
	struct SFeatures img1;
	struct SFeatures img2;

	vector<struct Matchespp> v_serialMatch;

	int n = dir.getNBImages() * (dir.getNBImages() - 1) / 2;
	int prog = 0;
	int NT = 0;

	string file(dir.getPath());
	file.append(MATCHFILE);

	FILE* f = fopen(file.c_str(), "wb");

	while (file[file.size() - 1] != '/')
	{
		file.pop_back();
	}

	int NBImages = dir.getNBImages();

	cout << endl;

	cout << "--> Matching : " << endl;

	for(int i = 0; i < NBImages; i++)
	{
		file.append(dir.getImage(i));

		while (file[file.size() - 1] != '.')
		{
			file.pop_back();
		}

		file.append("key");

		readAndAdjustSiftFile(dir.getPath(), dir.getImage(i), file, img1);

		while (file[file.size() - 1] != '/')
		{
			file.pop_back();
		}

		for(int j = 0; j < i; j++)
		{
			file.append(dir.getImage(j));

			while (file[file.size() - 1] != '.')
			{
				file.pop_back();
			}

			file.append("key");

			readAndAdjustSiftFile(dir.getPath(), dir.getImage(j), file, img2);

			struct Matchespp container(j, i);

			if (doMatch(img1, img2, container, geo)) NT++;

			//cout << container.NM << " match(es) found between " << dir.getImage(j) << " and " << dir.getImage(i) << endl;

			if (container.NM) v_serialMatch.push_back(container);

			while (file[file.size() - 1] != '/')
			{
				file.pop_back();
			}

			prog++;
			showProgress(prog, n, 75, 1);

		}
	}

	showProgress(n, n, 75, 0);

	cout << endl;

	cout << "--> Writing files :" << endl;

	if (geo) writeConstraints(dir.getPath(), v_serialMatch, n);
	else writeMatchFile(dir.getPath(), v_serialMatch, n);

	fclose(f);
}

/*
*	Function : matchMCore
*	Description : start to match point with OpenMPI on the given number of cores
*
*	path : working directory
*	numcore : number of cores
*/
void matchMCore(const string &path, int numcore, int geo)
{
	stringstream c;
	if (geo)
		c << "mpirun -n " << numcore << " cDoMatch " << path;
	else
		c << "mpirun -n " << numcore << " cDoMatch " << path << " 1 0";

	string command = c.str();

	system(command.c_str());
}

/*
*	Function : matchCMCore
*	Description : start to match point with OpenMPI on the given number of cores and on the supercomputer
*
*	path : working directory
*	numcore : number of cores
*/
void matchMCCore(const string &path, int numcore, int seconds)
{
	stringstream c;

	printf("--> Create the script : \n");

	createSubmit(path, numcore, seconds, 1);

	printf("--> Launch the script : \n");

	c << "msub ulavalSub/submit.sh";

	string command = c.str();

	system(command.c_str());

	cout << "Process launch, you can enter the command \"watch -n 10 showq -u $USER\" to see the progression." << endl << endl;
}

/*
*	Function : boss
*	Description : code for the boss
*
*	numcore : number of cores
*	dir : directory information
*/
int* boss(int numcore, const util::Directory &dir)
{
	int* dis = createDist4Match(dir.getNBImages(), numcore);

	return dis;
}


/*
*	Function : serializeVector
*	Description : to serialize a vector
*
*	sender : vector to serialize
*	img1 : images 1 index
*	img2 : images 2 index
*/
float* serializeContainer(const struct Matchespp &container)
{
	float* serialTab;

	int s = 15 + container.NM * 2;

	serialTab = (float*) malloc(s * sizeof(float));

	serialTab[0] = (float) s;
	serialTab[1] = (float) container.idx[0];
	serialTab[2] = (float) container.idx[1];
	serialTab[3] = (float) container.NM;

	for(int i = 0; i < container.NM * 2; i+=2)
	{
		int idx = i + 4;
		int idx2 = i/2;
		serialTab[idx] = (float) container.matches[idx2].queryIdx;
		serialTab[idx+1] = (float) container.matches[idx2].trainIdx;
	}

	int seek = 4 + container.NM * 2;

	serialTab[seek] = (float) container.NI;

	const double* M = container.H.ptr<double>();

	serialTab[seek+1] = (float) M[0];
	serialTab[seek+2] = (float) M[1];
	serialTab[seek+3] = (float) M[2];
	serialTab[seek+4] = (float) M[3];
	serialTab[seek+5] = (float) M[4];
	serialTab[seek+6] = (float) M[5];
	serialTab[seek+7] = (float) M[6];
	serialTab[seek+8] = (float) M[7];
	serialTab[seek+9] = (float) M[8];

	serialTab[seek+10] = container.ratio;

	return serialTab;
}


/*
*	Function : endComm
*	Description : implementation to end a conversation
*
*	sender : sender (worker) ID
*/
void endComm(int sender)
{
	float endSignal = -1.0;
	MPI_Send(&endSignal, 1, MPI_FLOAT, sender, 0, MPI_COMM_WORLD);
}


/*
*	Function : worker
*	Description : code for the workers
*
*	dir : directory information
*	recv : relative information about distribution
*	geo : if to do geometry or not
*/
void worker(const util::Directory &dir, int* recv, int geo)
{
	int aim = recv[0];
	int end = recv[1];

	int netID, seek = 0, compute = 0, stop = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &netID);

	vector<string> list;

	listDir(dir, list);

	struct SFeatures keys1, keys2;

	float* serialMatch;

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

				serialMatch = serializeContainer(container);

				//cout << "[CORE " << netID << "]: " << container.NM << " match(es) found between " << dir.getImage(j) << " and " << dir.getImage(i) << endl;

				container.reset();

				MPI_Send(serialMatch, serialMatch[0], MPI_FLOAT, SECRETARY, 1, MPI_COMM_WORLD);

				free(serialMatch);
			}

			seek++;
			if(seek == end){compute=0;stop=1;}
		}
	}

	endComm(SECRETARY);
}


/*
*	Function : writeSerialMatch
*	Description : code to write in the file from a serial matches structure, without geo
*
*	f : file descriptor
*	container : contains matches information
*	n : number of images
*	bar : to print bar or not
*/
void writeSerialMatch(const string &path, const vector<float*> &container, int n, int bar)
{
	string file1(path);

	file1.append("matches.init.txt");

	FILE *f1 = fopen(file1.c_str(), "wb");

	int ni = ( 1 + sqrt( 1 + 8 * n ) ) / 2;

	for (int i = 0; i < ni; i++)
	{
		for (int j = 0; j < ni; j++)
		{
			int reverse;
			float* pter = searchIDX(i, j, container, &reverse);

			if (pter != NULL && !reverse)
			{
				int NM = (int) pter[3];

				fprintf(f1, "%d %d\n", (int) pter[1], (int) pter[2]);
				fprintf(f1, "%d\n", NM);

				int num = 4;

				for(int j = 0; j < NM; j++)
				{
					fprintf(f1, "%d %d\n", (int) pter[num], (int) pter[num + 1]);
					num += 2;
				}
			}
			else if (pter != NULL && reverse)
			{
				int NM = (int) pter[3];

				fprintf(f1, "%d %d\n", (int) pter[2], (int) pter[1]);
				fprintf(f1, "%d\n", NM);

				int num = 4;

				for(int j = 0; j < NM; j++)
				{
					fprintf(f1, "%d %d\n", (int) pter[num + 1], (int) pter[num]);
					num += 2;
				}
			}

			if (bar) showProgress(i * ni + j, ni * ni, 75, 1);
		}
	}

	if (bar) showProgress(ni * ni, ni * ni, 75, 0);

	fclose(f1);
}

/*
*	Function : searchIDX
*	Description : code to search the tab idx of a certain i and j match
*
*	i : first image
*	j : second image
*	container : vector of matches informations
*	reverse : indicate if found but in the reverse order
*
*	return : the serial information if found, else NULL
*/
float* searchIDX(int i, int j, const vector<float*> &container, int* reverse)
{
	int num = container.size();

	*reverse = 0;

	for(int a = 0; a < num; a++)
	{
		if((int) container[a][1] == i && (int) container[a][2] == j)
			return container[a];
		if((int) container[a][1] == j && (int) container[a][2] == i)
		{
			*reverse = 1;
			return container[a];
		}
	}

	return NULL;
}


/*
*	Function : writeSerialMatchespp
*	Description : code to write in the file from a serial matchespp structure
*
*	path : directory path
*	container : contains the matches information
*	n : number of images
*	bar : to print bar or not
*/
void writeSerialMatchespp(const string &path, const vector<float*> &container, int n, int bar)
{
	string file1(path);
	string file2(path);

	file1.append("matches.init.txt");
	file2.append("ulavalSFM.txt");

	FILE *f1 = fopen(file1.c_str(), "wb");

	FILE *f2 = fopen(file2.c_str(), "wb");

	int ni = ( 1 + sqrt( 1 + 8 * n ) ) / 2;

	for (int i = 0; i < ni; i++)
	{
		for (int j = 0; j < ni; j++)
		{
			int reverse;
			float* pter = searchIDX(i, j, container, &reverse);

			if (pter != NULL && !reverse)
			{
				int NM = (int) pter[3];

				fprintf(f1, "%d %d\n", (int) pter[1], (int) pter[2]);
				fprintf(f1, "%d\n", NM);

				int num = 4;

				for(int j = 0; j < NM; j++)
				{
					fprintf(f1, "%d %d\n", (int) pter[num], (int) pter[num + 1]);
					num += 2;
				}

				if(pter[num] > 0)
				{
					fprintf(f2, "%d %d\n", (int) pter[1], (int) pter[2]);

				    fprintf(f2, "%d\n", (int) pter[num]);
				    fprintf(f2, "%f\n", pter[num + 10]);

				    fprintf(f2, "%f %f %f %f %f %f %f %f %f\n", pter[num + 1], pter[num + 2],
				     	pter[num + 3], pter[num + 4], pter[num + 5], pter[num + 6],
				       	pter[num + 7], pter[num + 8], pter[num + 9]);
				}
			}
			else if (pter != NULL && reverse)
			{
				int NM = (int) pter[3];

				fprintf(f1, "%d %d\n", (int) pter[2], (int) pter[1]);
				fprintf(f1, "%d\n", NM);

				int num = 4;

				for(int j = 0; j < NM; j++)
				{
					fprintf(f1, "%d %d\n", (int) pter[num + 1], (int) pter[num]);
					num += 2;
				}

				if(pter[num] > 0)
				{
					fprintf(f2, "%d %d\n", (int) pter[2], (int) pter[1]);

			        fprintf(f2, "%d\n", 0);
			        fprintf(f2, "%f\n", 0.0);

			        fprintf(f2, "%f %f %f %f %f %f %f %f %f\n", pter[num + 1], pter[num + 4],
			        	pter[num + 7], pter[num + 2], pter[num + 5], pter[num + 8],
			        	pter[num + 3], pter[num + 6], pter[num + 9]);
				}
			}

			if (bar) showProgress(i * ni + j, ni * ni, 75, 1);
		}
	}

	if (bar) showProgress(ni * ni, ni * ni, 75, 0);

	fclose(f1);
	fclose(f2);
}

/*
*	Function : recvFromWorker
*	Description : recv implementation for the secretary
*/
float* recvFromWorker(vector<int> &list)
{
	MPI_Status status;
	float* serialMatch;
	float garbage;
	int sender, s;

	MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	int tag = status.MPI_TAG;
	sender = status.MPI_SOURCE;

	list.push_back(sender);

	if(tag > 0)
	{
		MPI_Get_count(&status, MPI_FLOAT, &s);

		serialMatch = (float*) malloc(s * sizeof(float));
		MPI_Recv(serialMatch, s, MPI_FLOAT, sender, 1, MPI_COMM_WORLD, &status);
	}
	else
	{
		MPI_Recv(&garbage, 1, MPI_FLOAT, sender, tag, MPI_COMM_WORLD, &status);
		serialMatch = NULL;
	}

	return serialMatch;
}

/*
*	Function : secretary
*	Description : code for the secretary
*
*	path : directory path
*	numcore : number of cores
*	n : number of images
*	bar : if to print the bar or not
*	geo : if to print geo information or not
*/
void secretary(const string &path, int numcore, int n, int bar, int geo)
{
	vector<float*> v_serialMatch;
	vector<int> list;

	printf("GEO : %d\n", geo);

	int end = 1, i = 0;

	string file(path);
	file.append(MATCHFILE);

	printf("--> Matching : \n");

	int tbar = n + numcore - 1;

	while(end < numcore)
	{
		float* serialMatch;

		serialMatch = recvFromWorker(list);
		if (serialMatch)
			v_serialMatch.push_back(serialMatch);
		else
			end++;
		i++;
		if (bar) showProgress(i, tbar, 75, 1);
	}

	if (bar) showProgress(n, tbar, 75, 0);

	cout << "--> Writing file : " << endl;

	if (geo)
		writeSerialMatchespp(path, v_serialMatch, n, bar);
	else
		writeSerialMatch(path, v_serialMatch, n, bar);

	cout << endl;
}
