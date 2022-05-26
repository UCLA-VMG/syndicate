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

    std::string GetDeviceSerial(Spinnaker::CameraPtr pCam);
    bool setResolution(Spinnaker::GenApi::INodeMap& nodeMap, int height, int width);
    bool setFps(Spinnaker::GenApi::INodeMap& nodeMap, float fps);
    bool setContinuousAcquisitionMode(Spinnaker::GenApi::INodeMap& nodeMap);
    bool configure(Spinnaker::CameraPtr pCam, Spinnaker::GenApi::INodeMap& nodeMap, float fps, int height, int width);
    int AcquireImages(Spinnaker::CameraPtr pCam, Spinnaker::GenApi::INodeMap& nodeMap, Spinnaker::GenApi::INodeMap& nodeMapTLDevice, int num_frames);
    Spinnaker::CameraPtr getCam(Spinnaker::SystemPtr system, Spinnaker::CameraList camList, std::string serialNumber);

    Spinnaker::SystemPtr system;
    Spinnaker::CameraList camList;
    Spinnaker::CameraPtr flir_cam;
    std::string cameraID;

};
