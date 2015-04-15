/*
*	File : libgeometry.h
*	Author : Émile Robitaille @ LERobot
*	Creation date : 2015, March 7th
*	Version : 1.0
*
*	Description : Functions relative to geometry
*/

#ifndef DOGEOMETRY
#define DOGEOMETRY

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "libsift.h"
#include "directory.h"

void pruneDoubleMatch(struct Constraints &container);
void pruneDoubleMatch(struct Matchespp &new_box, const struct Matchespp &box);
void fMatrixFilter(struct Constraints &container);
int fMatrixFilter(const std::vector<cv::KeyPoint> &keys1, const std::vector<cv::KeyPoint> &keys2, std::vector<cv::DMatch> &list, float treshold = 3.0);
void transformInfo(struct Constraints &container);
void transformInfo(const std::vector<cv::KeyPoint> &keys1, const std::vector<cv::KeyPoint> &keys2, struct Matchespp &list, float treshold = 6.0);


#endif
