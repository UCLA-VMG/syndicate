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
string GetDeviceSerial(CameraPtr pCam) {
    INodeMap& nodeMap = pCam->GetTLDeviceNodeMap();
    CStringPtr ptrDeviceSerialNumber = nodeMap.GetNode("DeviceSerialNumber");
    if (IsReadable(ptrDeviceSerialNumber)) {
        return string(ptrDeviceSerialNumber->GetValue());
    }
    return "";
}

bool ConfigureCamera(CameraPtr pCam) {
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

// Use the following enum and global constant to select the type of video
// file to be created and saved.
enum videoType {
    UNCOMPRESSED,
    MJPG,
    H264
};
const videoType chosenVideoType = UNCOMPRESSED;

// This function prepares, saves, and cleans up an video from a vector of images.
int SaveVectorToVideo(INodeMap& nodeMap, INodeMap& nodeMapTLDevice, vector<ImagePtr>& images) {
    int result = 0;
    cout << endl << endl << "*** CREATING VIDEO ***" << endl << endl;
    try {
        // Retrieve device serial number for filename
        string deviceSerialNumber = "";
        CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial)) {
            deviceSerialNumber = ptrStringSerial->GetValue();
            cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
        }

        // Get the current frame rate; acquisition frame rate recorded in hertz
        CFloatPtr ptrAcquisitionFrameRate = nodeMap.GetNode("AcquisitionFrameRate");
        if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate)) {
            cout << "Unable to retrieve frame rate. Aborting..." << endl << endl;
            return -1;
        }
        float frameRateToSet = static_cast<float>(ptrAcquisitionFrameRate->GetValue());
        cout << "Frame rate to be set to " << frameRateToSet << "..." << endl;

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
            option.frameRate = frameRateToSet;
            option.height = static_cast<unsigned int>(images[0]->GetHeight());
            option.width = static_cast<unsigned int>(images[0]->GetWidth());
            video.Open(videoFilename.c_str(), option);
        }
        else if (chosenVideoType == MJPG) {
            Video::MJPGOption option;
            option.frameRate = frameRateToSet;
            option.quality = 75;
            option.height = static_cast<unsigned int>(images[0]->GetHeight());
            option.width = static_cast<unsigned int>(images[0]->GetWidth());
            video.Open(videoFilename.c_str(), option);
        }
        else if (chosenVideoType == H264) {
            Video::H264Option option;
            option.frameRate = frameRateToSet;
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

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(INodeMap& nodeMap) {
    int result = 0;
    cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;
    try {
        FeatureList_t features;
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category)) {
            category->GetFeatures(features);
            FeatureList_t::const_iterator it;
            for (it = features.begin(); it != features.end(); ++it) {
                CNodePtr pfeatureNode = *it;
                cout << pfeatureNode->GetName() << " : ";
                CValuePtr pValue = (CValuePtr)pfeatureNode;
                cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
                cout << endl;
            }
        }
        else {
            cout << "Device control information not available." << endl;
        }
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }
    return result;
}

// This function acquires and saves 30 images from a device; please see
// Acquisition example for more in-depth comments on acquiring images.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, vector<ImagePtr>& images) {
    int result = 0;
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
            cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
            return -1;
        }
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous)) {
            cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl
                 << endl;
            return -1;
        }
        int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
        cout << "Acquisition mode set to continuous..." << endl;
        // Begin acquiring images
        pCam->BeginAcquisition();
        cout << "Acquiring images..." << endl << endl;
        // Retrieve and convert images
        const unsigned int k_numImages = 30;
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

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunSingleCamera(CameraPtr pCam)
{
    int result = 0;
    int err = 0;
    try {
        // Retrieve TL device nodemap and print device information
        INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
        result = PrintDeviceInfo(nodeMapTLDevice);
        // Initialize camera
        pCam->Init();
        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();
        // Acquire images and save into vector
        vector<ImagePtr> images;
        err = AcquireImages(pCam, nodeMap, images);
        if (err < 0) {
            return err;
        }
        // Save vector of images to video
        result = result | SaveVectorToVideo(nodeMap, nodeMapTLDevice, images);
        // Deinitialize camera
        pCam->DeInit();
    }
    catch (Spinnaker::Exception& e) {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }
    return result;
}

// Example entry point; please see Enumeration example for more in-depth
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/) {
    int result = 0;
    // Print application build information
    cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;
    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();
    // Print out current library version
    const LibraryVersion spinnakerLibraryVersion = system->GetLibraryVersion();
    cout << "Spinnaker library version: " << spinnakerLibraryVersion.major << "." << spinnakerLibraryVersion.minor
         << "." << spinnakerLibraryVersion.type << "." << spinnakerLibraryVersion.build << endl
         << endl;
    // Retrieve list of cameras from the system
    CameraList camList = system->GetCameras();
    unsigned int numCameras = camList.GetSize();
    cout << "Number of cameras detected: " << numCameras << endl << endl;
    // Finish if there are no cameras
    if (numCameras == 0) {
        camList.Clear();
        system->ReleaseInstance();
        cout << "Not enough cameras!" << endl;
    }

    // Configure nir connected camera to use User Set 1 and register image events
    CameraPtr cam_nir = nullptr;
    for (unsigned int camIndex = 0; camIndex < camList.GetSize(); camIndex++) {
        CameraPtr cam = camList[camIndex];
        const string deviceSerialNumber = GetDeviceSerial(cam);
        if(deviceSerialNumber == "21290846"){
            cam_nir = camList[camIndex];
            cam = nullptr;
        }
    }
    if (!ConfigureCamera(cam_nir)) {
        cout << "Camera configuration for device " << GetDeviceSerial(cam_nir) << " unsuccessful, aborting...";
        return -1;
    }
    
    cout << endl << "Running example for camera " << GetDeviceSerial(cam_nir) << "..." << endl;
    result = result | RunSingleCamera(cam_nir);
    cout << "Camera example complete..." << endl << endl;

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    // system->ReleaseInstance();

    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}