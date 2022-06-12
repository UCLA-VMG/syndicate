#pragma once

#include "camera.h"
#include "StreamSystemInfo.h"
#include "ErrorCodeToMessage.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <iostream>
#include <iomanip>
#include <map>
#include <cstring>
#include <string>
#include <unordered_map>
#include <any>

#include <VimbaCPP/Include/VimbaCPP.h>
#include <VimbaImageTransform/Include/VmbTransform.h>

#include "CameraObserver.h"
#include "FrameObserver.h"
#include "Bitmap.h"

struct VimbaCamera : public Syndicate::Camera 
{
    VimbaCamera(std::unordered_map<std::string, std::any>& sample_config);
    ~VimbaCamera();

    // void AcquireSave(AVT::VmbAPI::CameraPtr m_pCamera, double seconds, std::string filename_prefix);
    void VimbaCamera::AcquireSave(double seconds);
    // void AcquireSaveBarrier(AVT::VmbAPI::CameraPtr m_pCamera, double seconds, std::string filename_prefix, boost::barrier& frameBarrier);
    void VimbaCamera::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);

    VmbErrorType setPixelFormatMono8(AVT::VmbAPI::CameraPtr m_pCamera);
    VmbErrorType setContinuousAcquisitonMode(AVT::VmbAPI::CameraPtr m_pCamera);
    VmbErrorType setFps(AVT::VmbAPI::CameraPtr m_pCamera, float fps);
    VmbErrorType setHeight(AVT::VmbAPI::CameraPtr m_pCamera, int height);
    VmbErrorType setWidth(AVT::VmbAPI::CameraPtr m_pCamera, int width);

    AVT::VmbAPI::CameraPtrVector GetCameraList();
    std::string GetCameraID();
    AVT::VmbAPI::CameraPtr getCam(const std::string& rStrCameraID);
    int GetWidth();
    int GetHeight();
    VmbPixelFormatType GetPixelFormat();
    AVT::VmbAPI::FramePtr GetFrame();
    AVT::VmbAPI::CameraObserver* GetCameraObserver();
    AVT::VmbAPI::FrameObserver* GetFrameObserver();

    VmbErrorType saveBitMap(AVT::VmbAPI::FramePtr& rpFrame, const char* pFileName);
    bool AcquireImages(AVT::VmbAPI::CameraPtr m_pCamera, const int num_frames);
    VmbErrorType configure(AVT::VmbAPI::CameraPtr m_pCamera, double fps, int height, int width);
    bool FrameAvailable();
    void closeCam(AVT::VmbAPI::CameraPtr m_pCamera);
    VmbErrorType StopContinuousImageAcquisition();

    AVT::VmbAPI::CameraPtr openCam(const std::string& rStrCameraID);

    std::string cameraID;
    // A reference to our Vimba singleton
    AVT::VmbAPI::VimbaSystem&                m_system;
    // The currently streaming camera
    AVT::VmbAPI::CameraPtr                   m_pCamera;
    // Every camera has its own frame observer
    AVT::VmbAPI::IFrameObserverPtr           m_pFrameObserver;
    // Our camera observer
    AVT::VmbAPI::ICameraListObserverPtr      m_pCameraObserver;
    // The current pixel format
    VmbInt64_t                  m_nPixelFormat;
    // The current width
    VmbInt64_t                  m_nWidth;
    // The current height
    VmbInt64_t                  m_nHeight;
};

