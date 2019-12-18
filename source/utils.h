#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

using namespace  cv;

bool matLine2du(Vec4d line1,Vec4d line2,double & mu, int v0=512);

#endif // UTILS_H
