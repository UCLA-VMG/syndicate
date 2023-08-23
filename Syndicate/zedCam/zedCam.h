#pragma once

#include "camera.h"
#include <stdio.h>
#include <windows.h>
#include <ctime>
#include <sl/Camera.hpp>

#include <tiff.h>
#include <tiffio.h>

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <iostream>
#include <iomanip>
#include <map>

using namespace sl;

struct zedCam : public Syndicate::Camera 
{
    zedCam(ptree::value_type& sensor_settings, ptree::value_type& global_settings);
    ~zedCam();

    void AcquireSave(double seconds, boost::barrier& startBarrier);

    int cameraID;
    // Create a ZED camera
    sl::Camera zed;
    bool hardware_sync;

};

const std::string currentDateTime();
