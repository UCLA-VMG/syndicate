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

// This helper function allows the example to sleep in both Windows and Linux
// systems. Note that Windows sleep takes milliseconds as a parameter while
// Linux systems take microseconds as a parameter.
void SleepyWrapper(int milliseconds) {
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    Sleep(milliseconds);
#else
    usleep(1000 * milliseconds);
#endif
}

// Provide the function signature for RunSingleCamera so it can be triggered on a
// device arrival.
bool ConfigureCamera(CameraPtr pCam);

// This function configures newly discovered cameras with desired settings, saves
// them to User Set 1 and sets this as the default. This way cameras will not need
// to be reconfigured if they are disconnected during the example. Note that this may
// overwrite current settings for User Set 1; please see ImageFormatControl example
// for more in-depth comments on camera configuration.
bool ConfigureUserSet1(CameraPtr pCam) {
    bool result = true;
    float fps = 30.0;
    int height = 640;
    int width = 640;

    try {
        // Get the camera node map
        INodeMap& nodeMap = pCam->GetNodeMap();

        // Get User Set 1 from the User Set Selector
        CEnumerationPtr ptrUserSetSelector = nodeMap.GetNode("UserSetSelector");
        if (!IsWritable(ptrUserSetSelector)) {
            cout << "Unable to set User Set Selector to User Set 1 (node retrieval). Aborting..." << endl << endl;
            return false;
        }

        CEnumEntryPtr ptrUserSet1 = ptrUserSetSelector->GetEntryByName("UserSet1");
        if (!IsReadable(ptrUserSet1)) {
            cout << "Unable to set User Set Selector to User Set 1 (enum entry retrieval). Aborting..." << endl << endl;
            return false;
        }
        const int64_t userSet1 = ptrUserSet1->GetValue();
        // Set User Set Selector to User Set 1
        ptrUserSetSelector->SetIntValue(userSet1);

        // Set User Set Default to User Set 1
        // This ensures the camera will re-enumerate using User Set 1, instead of the default user set.
        CEnumerationPtr ptrUserSetDefault = nodeMap.GetNode("UserSetDefault");
        if (!IsWritable(ptrUserSetDefault)) {
            cout << "Unable to set User Set Default to User Set 1 (node retrieval). Aborting..." << endl << endl;
            return false;
        }
        ptrUserSetDefault->SetIntValue(userSet1);

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
        cout << "fps: "<< frameRateToSet << endl;
        
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

        // Set height
        CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
        if(!IsWritable(acquisition_frame_rate->GetAccessMode()) || !IsReadable(acquisition_frame_rate->GetAccessMode()) || ptrHeight->GetInc() == 0 || ptrHeight->GetMax() == 0) {
            cout << "Unable to read or write to Height. Aborting..." << endl;
            return false;
        }
        ptrHeight->SetValue(height);
        int heightToSet = static_cast<int>(ptrHeight->GetValue());
        cout << "Height: " << heightToSet << endl << endl;

        // Set width
        CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
        if(!IsWritable(acquisition_frame_rate->GetAccessMode()) || !IsReadable(acquisition_frame_rate->GetAccessMode()) || ptrHeight->GetInc() == 0 || ptrHeight->GetMax() == 0) {
            cout << "Unable to read or write to Width. Aborting..." << endl;
            return false;
        }
        ptrWidth->SetValue(width);
        int widthToSet = static_cast<int>(ptrWidth->GetValue());
        cout << "Width: " << widthToSet << endl << endl;

        // Execute User Set Save to save User Set 1
        CCommandPtr ptrUserSetSave = nodeMap.GetNode("UserSetSave");
        if (!ptrUserSetSave.IsValid()) {
            cout << "Unable to save Settings to User Set 1. Aborting..." << endl << endl;
            return false;
        }
        ptrUserSetSave->Execute();
    }
    catch (Exception& e) {
        cout << "Error configuring user set 1: " << e.what() << endl;
        result = false;
    }
    
    return result;
}

// This function resets the cameras default User Set to the Default User Set.
// Note that User Set 1 will retain the settings set during ConfigureUserSet1.
void ResetCameraUserSetToDefault(CameraPtr pCam) {
    try {
        // Get the camera node map
        INodeMap& nodeMap = pCam->GetNodeMap();

        // Get User Set Default from User Set Selector
        CEnumerationPtr ptrUserSetSelector = nodeMap.GetNode("UserSetSelector");
        if (!IsWritable(ptrUserSetSelector)) {
            cout << "Unable to set User Set Selector to Default (node retrieval). Aborting..." << endl << endl;
            return;
        }

        CEnumEntryPtr ptrUserSetDefaultEntry = ptrUserSetSelector->GetEntryByName("Default");
        if (!IsReadable(ptrUserSetDefaultEntry)) {
            cout << "Unable to set User Set Selector to Default (enum entry retrieval). Aborting..." << endl << endl;
            return;
        }
        const int64_t userSetDefault = ptrUserSetDefaultEntry->GetValue();

        // Set User Set Selector back to User Set Default
        ptrUserSetSelector->SetIntValue(userSetDefault);

        // Set User Set Default to User Set Default
        CEnumerationPtr ptrUserSetDefault = nodeMap.GetNode("UserSetDefault");
        if (!IsWritable(ptrUserSetDefault)) {
            cout << "Unable to set User Set Default to User Set 1 (node retrieval). Aborting..." << endl << endl;
            return;
        }
        ptrUserSetDefault->SetIntValue(userSetDefault);

        // Execute User Set Load to load User Set Default
        CCommandPtr ptrUserSetLoad = nodeMap.GetNode("UserSetLoad");
        if (!ptrUserSetLoad.IsValid()) {
            cout << "Unable to load Settings from User Set Default. Aborting..." << endl << endl;
            return;
        }
        ptrUserSetLoad->Execute();
    }
    catch (Exception& e) {
        cout << "Error resetting camera to use default user set: " << e.what() << endl;
    }
}

// This helper function retrieves the device serial number from the cameras nodemap.
string GetDeviceSerial(CameraPtr pCam)
{
    INodeMap& nodeMap = pCam->GetTLDeviceNodeMap();
    CStringPtr ptrDeviceSerialNumber = nodeMap.GetNode("DeviceSerialNumber");
    if (IsReadable(ptrDeviceSerialNumber)) {
        return string(ptrDeviceSerialNumber->GetValue());
    }
    return "";
}

// This function configures new cameras to use camera settings from User Set 1 and
// registers an image event handler on new and rediscovered cameras.
bool ConfigureCamera(CameraPtr pCam)
{
    bool result = true;
    try {
        // Initialize camera
        pCam->Init();
        // Get the device serial number
        const string deviceSerialNumber = GetDeviceSerial(pCam);
        // Add a new entry into the camera grab info map using the default GrabInfo constructor
        cout << "Configuring device " << deviceSerialNumber << endl;
        // Configure the new camera to use User Set 1
        // Return if it is unsuccessful
        if (!ConfigureUserSet1(pCam)) {
            return false;
        }
    }
    catch (Exception& e) {
        cout << "Error Running single camera: " << e.what() << endl;
        result = false;
    }
    return result;
}

// This function acquires and saves 10 images from a device.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice) {
    int result = 0;
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        // Retrieve enumeration node from nodemap
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
            cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Retrieve entry node from enumeration node
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous)) {
            cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Retrieve integer value from entry node
        const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
        // Set integer value from entry node as new value of enumeration node
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
        cout << "Acquisition mode set to continuous..." << endl;
        
        // Begin acquiring images
        pCam->BeginAcquisition();
        cout << "Acquiring images..." << endl;
        // Retrieve device serial number for filename
        gcstring deviceSerialNumber("");
        CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial)) {
            deviceSerialNumber = ptrStringSerial->GetValue();

            cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
        }
        cout << endl;
        // Retrieve, convert, and save images
        const unsigned int k_numImages = 60;
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
                    if (!deviceSerialNumber.empty())
                    {
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

int RunSingleCamera(CameraPtr pCam) {
    int result;
    try {
        // Retrieve TL device nodemap
        INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
        // Initialize camera
        pCam->Init();
        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();
        // Acquire images
        result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);
        // Reset camera user sets and deinitialize camera
        if (pCam.IsValid()) {
            ResetCameraUserSetToDefault(pCam);
        }
        pCam->DeInit();
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }
    
    return result;
}

// Example entry point; please see EnumerationEvents example for more in-depth
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
    float fps = 30.0;
    int height = 640;
    int width = 640;
    int acquisition_time = 5;
    int NUM_FRAMES = (static_cast<int>(fps))*(acquisition_time);

    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();
    // Retrieve list of cameras from the system
    CameraList globalCamList = system->GetCameras();
    int numCameras = globalCamList.GetSize();
    cout << "Number of cameras detected: " << numCameras << endl;
    if (numCameras == 0) {
        globalCamList.Clear();
        system->ReleaseInstance();
        cout << "Not enough cameras!" << endl;
    }

    // Configure nir connected camera to use User Set 1 and register image events
    CameraPtr cam_nir = nullptr;
    for (unsigned int camIndex = 0; camIndex < globalCamList.GetSize(); camIndex++) {
        CameraPtr cam = globalCamList[camIndex];
        const string deviceSerialNumber = GetDeviceSerial(cam);
        if(deviceSerialNumber == "21290846"){
            cam_nir = globalCamList[camIndex];
            cam = nullptr;
        }
    }
    if (!ConfigureCamera(cam_nir)) {
        cout << "Camera configuration for device " << GetDeviceSerial(cam_nir) << " unsuccessful, aborting...";
        return -1;
    }

    // Begin recording images
    cout << endl << "Press any key to begin..." << endl << endl;
    getchar();
    int result = 0;
    result = result | RunSingleCamera(cam_nir);
    cam_nir = nullptr;
    cout << endl;

    // Clear camera list before releasing system
    globalCamList.Clear();

    // Release system
    system->ReleaseInstance();
    cout << endl << "Done! Press any key to exit..." << endl;
    getchar();

    return 0;
}