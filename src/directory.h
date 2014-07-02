/*
*	File : directory.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2014, June 27th
*	Version : 1.0
*	
*	Description : directory class
*/

#ifndef DIRECTORY
#define DIRECTORY

#include <dirent.h>
#include <string>
#include <vector>


/* 
*	Class : Directory
*	Description : Class used to store directory info
*	
*	Methods :
*
*		Directory() : Default constructor
*		Directory(char* p_path) : Constructor who assign a directory directly  
*		void assign(char* p_path) : assign a directory
*		void update() : update the directory info
*		int getNBImages() const : get the number of images
*		int getNBSifts() const : get the number of .sift files
*		int getNBMatches() const : get the number of .match files
*		string getPath() const : get the directory path name
*		string getImage(int i) const : get the image path name
*		string getSift(int i) const : get the .sift file path name
*		string getMatch(int i) const : get the .match file path name
*		void printInfo() const : print a string with the attributes value
*		
*	Attributes :
*
*		String m_path : the directory path name
*		vector<string> m_images : vector storing all images path
*		vector<string> m_sifts : vector storing all .sift files path
*		vector<string> m_matches : vector storing all .match files path
*/
namespace util
{
class Directory
{

public:

	Directory();
	Directory(char* p_path);

	void assign(char* p_path);
	void update();
	int getNBImages() const;
	int getNBSifts() const;
	int getNBMatches() const;
	std::string getPath() const;
	std::string getImage(int i) const;
	std::string getSift(int i) const;
	std::string getMatch(int i) const;
	void printInfo() const;

private:

	std::string m_path;
	std::vector<std::string> m_images;
	std::vector<std::string> m_sifts;
	std::vector<std::string> m_matches;

};
}; // namespace util


#endif
