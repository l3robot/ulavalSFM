/*
*	File : directory.cpp
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*
*	Description : directory class
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <string>
#include <iostream>
#include <dirent.h>

#include "directory.h"
#include "util.h"

using namespace std;

namespace util
{
Directory::Directory()
{}

Directory::Directory(const char* p_path)
{
	m_path.assign(p_path);

	if(m_path[m_path.size()-1] != '/') m_path.append(1, '/');

	DIR* dir = opendir(m_path.c_str());
	struct dirent* file;

	if (!dir)
	{
		cout << "Forced to quit. Reason : No directory named \"" << p_path << "\"" << endl;
		exit(1);
	}

	while((file = readdir(dir)))
	{
		if(strstr(file->d_name, ".jpg") || strstr(file->d_name, ".JPG"))
		{
			m_images.push_back(string(file->d_name));
		}
		else if(strstr(file->d_name, ".key"))
		{
			m_sifts.push_back(string(file->d_name));
		}
		else if(strstr(file->d_name, ".init.txt"))
		{
			m_matches.push_back(string(file->d_name));
		}
	}

	closedir(dir);

	createIMGlist();
}

void Directory::assign(const char* p_path)
{
	m_path.assign(p_path);

	if(m_path[m_path.size()-1] != '/') m_path.append(1, '/');

	DIR* dir = opendir(p_path);
	struct dirent* file;

	if (!dir)
	{
		cout << "Forced to quit. Reason : No directory named \"" << p_path << "\"" << endl;
		exit(1);
	}

	while((file = readdir(dir)))
	{
		if(strstr(file->d_name, ".jpg") || strstr(file->d_name, ".JPG"))
		{
			m_images.push_back(string(file->d_name));
		}
		else if(strstr(file->d_name, ".key"))
		{
			m_sifts.push_back(string(file->d_name));
		}
		else if(strstr(file->d_name, ".init.txt"))
		{
			m_matches.push_back(string(file->d_name));
		}
	}

	closedir(dir);
}

void Directory::update()
{
	m_images.clear();
	m_sifts.clear();
	m_matches.clear();

	DIR* dir = opendir(m_path.c_str());
	struct dirent* file;

	if (!dir)
	{
		cout << "Forced to quit. Reason : No directory named \"" << m_path << "\"" << endl;
		exit(1);
	}

	while((file = readdir(dir)))
	{
		if(strstr(file->d_name, ".jpg") || strstr(file->d_name, ".JPG"))
		{
			m_images.push_back(string(file->d_name));
		}
		else if(strstr(file->d_name, ".key"))
		{
			m_sifts.push_back(string(file->d_name));
		}
		else if(strstr(file->d_name, ".init.txt"))
		{
			m_matches.push_back(string(file->d_name));
		}
	}

	closedir(dir);
}

void Directory::createIMGlist()
{
	string file(m_path);

	file.append(IMGLIST);

	FILE* f = fopen(file.c_str(), "wb");

	string img("./");

	for (int i = 0; i < m_images.size(); i++)
	{
		img.append(getImage(i));

		fprintf(f, "%s\n", img.c_str());

		while (img[img.size() - 1] != '/')
		{
			img.pop_back();
		}
	}

	fclose(f);
}

int Directory::getNBImages() const
{
	return m_images.size();
}

int Directory::getNBSifts() const
{
	return m_sifts.size();
}

int Directory::getNBMatches() const
{
	return m_matches.size();
}

string Directory::getPath() const
{
	return m_path;
}

string Directory::getImage(int i) const
{
	return m_images[i];
}

string Directory::getSift(int i) const
{
	return m_sifts[i];
}

string Directory::getMatch(int i) const
{
	return m_matches[i];
}

void Directory::printInfo() const
{
	cout << endl;
	cout << "Directory Info" << endl;
	cout << "--------------" << endl << endl;

	cout << "Name                    : " << m_path << endl;
	cout << "Number of image(s)      : " << m_images.size() << endl;
	cout << "Number of sift file(s)  : " << m_sifts.size() << endl;
	cout << "Number of match file(s) : " << m_matches.size() << endl << endl;
}
}; //namespace util
