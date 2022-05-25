#ifndef THERMAL
#define THERMAL

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <string>
#include <windows.h> // For Sleep
#include <iostream>
#include <chrono>
#include <ctime> 

using namespace cv;
using namespace std;

void get_fps(VideoCapture cap);
void get_resolution(VideoCapture cap);
void set_fps(VideoCapture cap, double fps);
void set_resolution(VideoCapture cap, double width, double height);
const string currentDateTime();

#endif
