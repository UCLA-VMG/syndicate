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
 *  @example SaveToAvi.cpp
 *
 *  @brief SaveToAvi.cpp shows how to create an video from a vector of images.
 *  It relies on information provided in the Enumeration, Acquisition, and
 *  NodeMapInfo examples.
 *
 *  This example introduces the SpinVideo class, which is used to quickly and
 *  easily create various types of video files. It demonstrates the creation of
 *  three types: uncompressed, MJPG, and H264.
 */

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include "SpinVideo.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;
using namespace std;

// This helper function retrieves the device serial number from the cameras nodemap.
string GetDeviceSerial(INodeMap& nodeMapTLDevice) {
    CStringPtr ptrDeviceSerialNumber = nodeMapTLDevice.GetNode("DeviceSerialNumber");
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

bool enableUserSet1(INodeMap& nodeMap) {
    bool result = true;
    try {
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
    }
        catch (Exception& e) {
        cout << "Error configuring fps: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool saveUserSet1(INodeMap& nodeMap) {
    bool result = true;
    try {
        // Execute User Set Save to save User Set 1
        CCommandPtr ptrUserSetSave = nodeMap.GetNode("UserSetSave");
        if (!ptrUserSetSave.IsValid()) {
            cout << "Unable to save Settings to User Set 1. Aborting..." << endl << endl;
            return false;
        }
        ptrUserSetSave->Execute();
    }
        catch (Exception& e) {
        cout << "Error configuring fps: " << e.what() << endl;
        result = false;
    }
    return result;
}

// This function resets the cameras default User Set to the Default User Set.
// Note that User Set 1 will retain the settings set during ConfigureUserSet1.
void ResetCameraUserSetToDefault(INodeMap& nodeMap) {
    try {
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

bool configure(INodeMap& nodeMap, float fps = 30.0, int height = 640, int width = 640) {
    bool result = true;
    try {
        // enable User Set 1
        if (!enableUserSet1(nodeMap)) {
            return false;
        }
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
        // Execute User Set Save to save User Set 1
        if (!saveUserSet1(nodeMap)) {
            return false;
        }
    }
    catch (Exception& e) {
        cout << "Error configuring camera: " << e.what() << endl;
        result = false;
    }
    return result;
}

// Use the following enum and global constant to select the type of video
// file to be created and saved.
enum videoType {
    UNCOMPRESSED,
    MJPG,
    H264
};

// This function prepares, saves, and cleans up an video from a vector of images.
int SaveVectorToVideo(INodeMap& nodeMap, INodeMap& nodeMapTLDevice, vector<ImagePtr>& images, videoType chosenVideoType, float fps) {
    int result = 0;
    cout << endl << endl << "*** CREATING VIDEO ***" << endl << endl;
    try {
        // Retrieve device serial number for filename
        string deviceSerialNumber = GetDeviceSerial(nodeMapTLDevice);
        cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;

        // Create a unique filename
        string videoFilename;
        switch (chosenVideoType) {
            case UNCOMPRESSED:
                videoFilename = "SaveToAvi-Uncompressed";
                if (deviceSerialNumber != "") {
                    videoFilename = videoFilename + "-" + deviceSerialNumber.c_str();
                }
                break;
            case MJPG:
                videoFilename = "SaveToAvi-MJPG";
                if (deviceSerialNumber != "") {
                    videoFilename = videoFilename + "-" + deviceSerialNumber.c_str();
                }
                break;
            case H264:
                videoFilename = "SaveToAvi-H264";
                if (deviceSerialNumber != "") {
                    videoFilename = videoFilename + "-" + deviceSerialNumber.c_str();
                }
        }

        // Select option and open video file type
        SpinVideo video;
        const unsigned int k_videoFileSize = 2048;
        video.SetMaximumFileSize(k_videoFileSize);
        if (chosenVideoType == UNCOMPRESSED) {
            Video::AVIOption option;
            option.frameRate = fps;
            option.height = static_cast<unsigned int>(images[0]->GetHeight());
            option.width = static_cast<unsigned int>(images[0]->GetWidth());
            video.Open(videoFilename.c_str(), option);
        }
        else if (chosenVideoType == MJPG) {
            Video::MJPGOption option;
            option.frameRate = fps;
            option.quality = 75;
            option.height = static_cast<unsigned int>(images[0]->GetHeight());
            option.width = static_cast<unsigned int>(images[0]->GetWidth());
            video.Open(videoFilename.c_str(), option);
        }
        else if (chosenVideoType == H264) {
            Video::H264Option option;
            option.frameRate = fps;
            option.bitrate = 1000000;
            option.height = static_cast<unsigned int>(images[0]->GetHeight());
            option.width = static_cast<unsigned int>(images[0]->GetWidth());
            video.Open(videoFilename.c_str(), option);
        }

        // Construct and save video
        cout << "Appending " << images.size() << " images to video file: " << videoFilename << ".avi... " << endl
             << endl;
        for (unsigned int imageCnt = 0; imageCnt < images.size(); imageCnt++) {
            video.Append(images[imageCnt]);
            cout << "\tAppended image " << imageCnt << "..." << endl;
        }

        // Close video file
        video.Close();
        cout << endl << "Video saved at " << videoFilename << ".avi" << endl << endl;
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }
    return result;
}

// This function acquires and saves 30 images from a device; please see
// Acquisition example for more in-depth comments on acquiring images.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, vector<ImagePtr>& images, int num_frames) {
    int result = 0;
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        // Begin acquiring images
        pCam->BeginAcquisition();
        cout << "Acquiring images..." << endl << endl;
        // Retrieve and convert images
        const unsigned int k_numImages = num_frames;
        for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
            try {
                // Retrieve the next received image
                ImagePtr pResultImage = pCam->GetNextImage(1000);
                if (pResultImage->IsIncomplete()) {
                    cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl
                         << endl;
                }
                else {
                    cout << "Grabbed image " << imageCnt << ", width = " << pResultImage->GetWidth()
                         << ", height = " << pResultImage->GetHeight() << endl;

                    // Deep copy image into image vector
                    images.push_back(pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR));
                }
                // Release image
                pResultImage->Release();
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
        result = -1;
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
    // Configure nir connected camera to use User Set 1 and register image events
    for (unsigned int camIndex = 0; camIndex < camList.GetSize(); camIndex++) {
        CameraPtr cam = camList[camIndex];
        INodeMap& nodeMapTLDevice = cam->GetTLDeviceNodeMap();
        const string deviceSerialNumber = GetDeviceSerial(nodeMapTLDevice);
        if(string(deviceSerialNumber) == serialNumber) {
            cout << "Found!" << endl;
            return cam;
        }
    }
    return nullptr;
}



// Example entry point; please see Enumeration example for more in-depth
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/) {
    // set configurations
    const videoType chosenVideoType = UNCOMPRESSED;
    float fps = 30.0;
    int height = 640;
    int width = 640;
    int acquisition_time = 10;
    int num_frames = acquisition_time*static_cast<int>(fps);
    string serialNumber = "21290846";
    // define result
    int result = 0;

    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();
    // Retrieve list of cameras from the system
    CameraList camList = system->GetCameras();
    // Retrieve chosen camera using predetermined serial number
    CameraPtr nir_cam = getCam(system, camList, serialNumber);
    // Initialize camera
    nir_cam->Init();
    // Get NodeMap and nodeMapTLDevice
    INodeMap& nodeMap = nir_cam->GetNodeMap();
    INodeMap& nodeMapTLDevice = nir_cam->GetTLDeviceNodeMap();
    // predefine images vector
    vector<ImagePtr> images;
    
    try {
        // Configure camera
        if (!configure(nodeMap, fps, height, width)) {
            cout << "Camera configuration for device " << serialNumber << " unsuccessful, aborting...";
            return -1;
        }
        // Acquire images and save into vector
        int err = AcquireImages(nir_cam, nodeMap, images, num_frames);
        if (err < 0) {
            cout << "Acquiring images for device " << serialNumber << " unsuccessful, aborting...";
            return err;
        }
        // Save vector of images to video
        result = result | SaveVectorToVideo(nodeMap, nodeMapTLDevice, images, chosenVideoType, fps);
        // Reset camera user sets
        if (nir_cam.IsValid()) {
            ResetCameraUserSetToDefault(nodeMap);
        }
        // Deinitialize camera
        nir_cam->DeInit();
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    // Clear camera list before releasing system
    camList.Clear();
    // // Release system
    // system->ReleaseInstance();
    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}