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

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);

    Spinnaker::SystemPtr system;
    Spinnaker::CameraList camList;
    Spinnaker::CameraPtr flir_cam;
    std::string cameraID;

};

// using namespace Spinnaker;
// using namespace Spinnaker::GenApi;
// using namespace Spinnaker::GenICam;

Spinnaker::CameraPtr getCam(Spinnaker::SystemPtr system, Spinnaker::CameraList camList, std::string serialNumber);

bool configure(Spinnaker::CameraPtr pCam, Spinnaker::GenApi::INodeMap& nodeMap, float fps, int height, int width);

std::string GetDeviceSerial(Spinnaker::CameraPtr pCam);

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);