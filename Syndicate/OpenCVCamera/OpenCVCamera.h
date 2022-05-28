#pragma once

#include "camera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <windows.h>
#include <ctime>

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <iostream>
#include <iomanip>
#include <map>

struct OpenCVCamera : public Syndicate::Camera 
{
    OpenCVCamera(std::unordered_map<std::string, std::any>& sample_config);
    ~OpenCVCamera();

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    bool setResolution(cv::VideoCapture cap, double width, double height);
    bool setFps(cv::VideoCapture cap, double fps);
    bool AcquireImages(cv::VideoCapture cap, int num_frames);
    bool getResolution(cv::VideoCapture cap);
    bool getFps(cv::VideoCapture cap);

    cv::VideoCapture openCap(int cameraID);
    bool configure(cv::VideoCapture cap, int deviceID, double fps, double height, double width);

    int cameraID;
    cv::VideoCapture cap;
    cv::Mat frame;
    // string filename;
    // double fps;
    // double height;
    // double width;

};

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
const std::string currentDateTime();