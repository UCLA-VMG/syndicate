#pragma once

#include "camera.h"
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <iostream>
#include <iomanip>
#include <map>

struct SpinnakerCamera : public Syndicate::Camera 
{
    SpinnakerCamera(std::unordered_map<std::string, std::any>& sample_config);
    ~SpinnakerCamera();

    void AcquireSave(double seconds, boost::barrier& startBarrier);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    Spinnaker::SystemPtr system;
    Spinnaker::CameraList camList;
    Spinnaker::CameraPtr flir_cam;
    
    std::string cameraID;
    std::string cameraType;
    
    double fps;
    int height;
    int width;
    std::string pixelFormat;
    
    int height_bin;
    int width_bin;
    double exposure_compensation;
    double exposure_time;
    double gain;
    double black_level;
};

// using namespace Spinnaker;
// using namespace Spinnaker::GenApi;
// using namespace Spinnaker::GenICam;

Spinnaker::CameraPtr getCam(Spinnaker::SystemPtr system, Spinnaker::CameraList camList, std::string serialNumber);

bool configure(Spinnaker::CameraPtr pCam, Spinnaker::GenApi::INodeMap& nodeMap, std::string cameraType, double fps, int height, int width, std::string pixelFormat, int height_bin, int width_bin, double exposure_compensation, double exposure_time, double gain, double black_level);

std::string GetDeviceSerial(Spinnaker::CameraPtr pCam);

bool setPrimary(Spinnaker::GenApi::INodeMap& nodeMap, std::string& cameraName);

bool setSecondary(Spinnaker::GenApi::INodeMap& nodeMap);

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);