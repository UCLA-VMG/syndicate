#ifndef THERMAL
#define THERMAL

#include "opencv2/cv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <string>
#include <windows.h> // For Sleep
#include <iostream>

using namespace cv;
using namespace std;

void getCam(const int camID);
void setPixelFormat(const int camID, string pixelFormat);
void setHeight(const int camID, const int height);
void setWidth(const int camID, const int width);
void setFps(const int camID, const float fps);
// void setContinuousAcquisitonMode(CameraPtr m_pCamera);




#endif
