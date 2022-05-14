//=============================================================================
// Copyright (c) 2001-2021 FLIR Systems, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

/**
 *  @example AcquisitionMultipleCameraRecovery.cpp
 *
 *  @brief AcquisitionMultipleCameraRecovery.cpp shows how to continously acquire
 *  images from multiple cameras using image events. It demonstrates the use of
 *  User Set Control to save persistent camera configurations, allowing for smooth
 *  camera recovery through interface events. This example relies on information
 *  provided in the ImageEvents, EnumerationEvents, ImageFormatControl, and
 *  Acquisition examples.
 *
 *  This example uses a global map to retain image information, including the number
 *  of images grabbed, the number of incomplete images and the number of removals
 *  for each camera over the duration of the example. Cameras may be added or
 *  removed after the example has started.
 *
 *  The example assumes each camera has a unique serial number and is capable of
 *  configuring User Set 1. Note that if a camera was configured and is disconnected
 *  before the example ends, it will not be reconfigured to use the default User Set.
 */

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
// #include <OpenImageIO/imageio.h>

#include <iostream>
#include <iomanip>
#include <map>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

// This helper function retrieves the device serial number from the cameras nodemap.
string GetDeviceSerial(CameraPtr pCam) {
    INodeMap& nodeMap = pCam->GetTLDeviceNodeMap();
    CStringPtr ptrDeviceSerialNumber = nodeMap.GetNode("DeviceSerialNumber");
    if ((IsAvailable(ptrDeviceSerialNumber)) && (IsReadable(ptrDeviceSerialNumber))) {
        cout << string(ptrDeviceSerialNumber->GetValue()) << endl;
        return string(ptrDeviceSerialNumber->GetValue());
    }
    return "";
}

bool setResolution(INodeMap& nodeMap, int height, int width) {
    bool result = true;
    try {
        // Set height
        CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
        if(!IsWritable(ptrHeight->GetAccessMode()) || !IsReadable(ptrHeight->GetAccessMode()) || ptrHeight->GetInc() == 0 || ptrHeight->GetMax() == 0) {
            cout << "Unable to read or write to Height. Aborting..." << endl;
            return false;
        }
        ptrHeight->SetValue(height);
        int heightToSet = static_cast<int>(ptrHeight->GetValue());
        cout << "Height: " << heightToSet << endl << endl;

        // Set width
        CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
        if(!IsWritable(ptrWidth->GetAccessMode()) || !IsReadable(ptrWidth->GetAccessMode()) || ptrWidth->GetInc() == 0 || ptrWidth->GetMax() == 0) {
            cout << "Unable to read or write to Width. Aborting..." << endl;
            return false;
        }
        ptrWidth->SetValue(width);
        int widthToSet = static_cast<int>(ptrWidth->GetValue());
        cout << "Width: " << widthToSet << endl << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring resolution: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool setFps(INodeMap& nodeMap, float fps) {
    bool result = true;
    try {
        // Set frame_rate_auto_node to false
        CEnumerationPtr frame_rate_auto_node = nodeMap.GetNode("AcquisitionFrameRateAuto");
        CEnumEntryPtr node_frame_rate_auto_off = frame_rate_auto_node->GetEntryByName("Off");
        const int64_t frame_rate_auto_off = node_frame_rate_auto_off->GetValue();
        frame_rate_auto_node->SetIntValue(frame_rate_auto_off);

        // Set frame_rate_enable to true
        CBooleanPtr enable_rate_mode = nodeMap.GetNode("AcquisitionFrameRateEnabled");
        if (!IsWritable(enable_rate_mode)) {
            cout << "enable_rate_mode not writable. Aborting..." << endl << endl;
            return false;
        }
        try {
            enable_rate_mode->SetValue(true);
        }
        catch(Exception& e) {
            std::cerr << e.what() << '\n'; // "Could not enable frame rate: {0}".format(ex)
        }
        CBooleanPtr acquisition_frame_rate_enable = nodeMap.GetNode("AcquisitionFrameRateEnable");

        // Set fps
        CFloatPtr acquisition_frame_rate = nodeMap.GetNode("AcquisitionFrameRate");
        if(!IsWritable(acquisition_frame_rate->GetAccessMode()) || !IsReadable(acquisition_frame_rate->GetAccessMode()) ) {
            cout << "Unable to set Frame Rate. Aborting..." << endl << endl;
            return false;
        }
        acquisition_frame_rate->SetValue(fps);
        float frameRateToSet = static_cast<float>(acquisition_frame_rate->GetValue());
        cout << "Frame rate to be set to " << frameRateToSet << "..." << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring fps: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool setContinuousAcquisitionMode(INodeMap& nodeMap) {
    bool result = true;
    try {
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsWritable(ptrAcquisitionMode)) {
            cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
            return false;
        }
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsReadable(ptrAcquisitionModeContinuous)) {
            cout << "Unable to set acquisition mode to continuous (enum entry retrieval). Aborting..." << endl << endl;
            return false;
        }
        const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
        cout << "ptrAcquisitionMode: "<< ptrAcquisitionMode->GetEntryByName("Continuous") << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring fps: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool configure(CameraPtr pCam, INodeMap& nodeMap, float fps = 30.0, int height = 640, int width = 640) {
    bool result = true;
    try {
        // Get the device serial number
        const string deviceSerialNumber = GetDeviceSerial(pCam);
        // Add a new entry into the camera grab info map using the default GrabInfo constructor
        cout << "Configuring device " << deviceSerialNumber << endl;

        // Set acquisition mode to continuous
        if (!setContinuousAcquisitionMode(nodeMap)) {
            return false;
        }
        // set fps
        if (!setFps(nodeMap, fps)) {
            return false;
        }
        // set resolution
        if (!setResolution(nodeMap, height, width)) {
            return false;
        }
    }
    catch (Exception& e) {
        cout << "Error configuring camera: " << e.what() << endl;
        result = false;
    }
    return result;
}

// This function acquires and saves 10 images from a device.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, int num_frames) {
    int result = 0;
    const string deviceSerialNumber = GetDeviceSerial(pCam);
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        // Begin acquiring images
        pCam->BeginAcquisition();
        cout << "Acquiring images..." << endl << endl;
        // Retrieve, convert, and save images
        const unsigned int k_numImages = num_frames;
        for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
            try {
                // Retrieve next received image
                ImagePtr pResultImage = pCam->GetNextImage(1000);
                // Ensure image completion
                if (pResultImage->IsIncomplete()) {
                    // Retrieve and print the image status description
                    cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                         << "..." << endl
                         << endl;
                }
                else {
                    // Convert image to mono 8
                    ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);
                    // Create a unique filename
                    ostringstream filename;
                    filename << "Acquisition-";
                    if (!deviceSerialNumber.empty()) {
                        filename << deviceSerialNumber.c_str() << "-";
                    }
                    filename << imageCnt << ".jpg";
                    // Save image
                    convertedImage->Save(filename.str().c_str());
                    cout << "Image saved at " << filename.str() << endl;
                }
                // Release image
                pResultImage->Release();
                cout << endl;
            }
            catch (Spinnaker::Exception& e) {
                cout << "Error: " << e.what() << endl;
                result = -1;
            }
        }
        // End acquisition
        pCam->EndAcquisition();
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        return -1;
    }
    return result;
}

CameraPtr getCam(SystemPtr system, CameraList camList, string serialNumber) {
    unsigned int numCameras = camList.GetSize();
    cout << "Number of cameras detected: " << numCameras << endl << endl;
    // Finish if there are no cameras
    if (numCameras == 0) {
        camList.Clear();
        system->ReleaseInstance();
        cout << "Not enough cameras!" << endl;
    }
    // Configure polarized connected camera to use User Set 1 and register image events
    for (unsigned int camIndex = 0; camIndex < camList.GetSize(); camIndex++) {
        CameraPtr cam = camList[camIndex];
        const string deviceSerialNumber = GetDeviceSerial(cam);
        if(string(deviceSerialNumber) == serialNumber) {
            cout << "Found!" << endl;
            return cam;
        }
    }
    return nullptr;
}

// Example entry point; please see EnumerationEvents example for more in-depth
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
    // set configurations
    float fps = 30.0;
    int height = 640;
    int width = 640;
    int acquisition_time = 10;
    int num_frames = acquisition_time*static_cast<int>(fps);
    string serialNumber = "19224369";
    // define result
    int result = 0;

    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();
    // Retrieve list of cameras from the system
    CameraList camList = system->GetCameras();
    // Retrieve chosen camera using predetermined serial number
    CameraPtr polarized_cam = getCam(system, camList, serialNumber);
    // Initialize camera
    polarized_cam->Init();
    // Get NodeMap and nodeMapTLDevice
    INodeMap& nodeMap = polarized_cam->GetNodeMap();
    INodeMap& nodeMapTLDevice = polarized_cam->GetTLDeviceNodeMap();

    try {
        // Configure camera
        if (!configure(polarized_cam, nodeMap, fps, height, width)) {
            cout << "Camera configuration for device " << serialNumber << " unsuccessful, aborting...";
            return -1;
        }
        // Acquire images and save into vector
        result = result | AcquireImages(polarized_cam, nodeMap, nodeMapTLDevice, num_frames);
        // Deinitialize camera
        polarized_cam->DeInit();
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }
    polarized_cam = nullptr;
    cout << endl;

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    system->ReleaseInstance();
    cout << endl << "Done! Press any key to exit..." << endl;
    getchar();

    return 0;
}