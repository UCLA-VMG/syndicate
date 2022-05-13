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
 *  @example NodeMapInfo.cpp
 *
 *  @brief NodeMapInfo.cpp shows how to retrieve node map information. It relies
 *  on information provided in the Enumeration example. Also, check out the
 *  Acquisition and ExceptionHandling examples if you haven't already.
 *  Acquisition demonstrates image acquisition while ExceptionHandling shows the
 *  handling of standard and Spinnaker exceptions.
 *
 *  This example explores retrieving information from all major node types on the
 *  camera. This includes string, integer, float, boolean, command, enumeration,
 *  category, and value types. Looping through multiple child nodes is also
 *  covered. A few node types are not covered - base, port, and register - as
 *  they are not fundamental. The final node type - enumeration entry - is
 *  explored and printed for nodes whose parent node is a selector node.
 *
 *  Once comfortable with NodeMapInfo, we suggest checking out ImageFormatControl
 *  and Exposure. ImageFormatControl explores customizing image settings on a
 *  camera while Exposure introduces the standard structure of configuring a
 *  device, acquiring some images, and then returning the device to a default
 *  state.
 */

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "SpinnakerInfo.h"
#include <iostream>
#include <sstream>

// Define the maximum number of characters that will be printed out
// for any information retrieved from a node.
const unsigned int maxChars = 35;

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

// This function acts as the body of the example. First nodes from the TL
// device and TL stream nodemaps are retrieved and printed. Following this,
// the camera is initialized and then nodes from the GenICam nodemap are
// retrieved and printed.

// Example entry point; please see Enumeration example for more in-depth
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
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
    if (numCameras == 0)
    {
        // Clear camera list before releasing system
        camList.Clear();

        // Release system
        system->ReleaseInstance();

        cout << "Not enough cameras!" << endl;
        cout << "Done! Press Enter to exit..." << endl;
        getchar();

        return -1;
    }

    //
    // Create shared pointer for camera
    //
    // *** NOTES ***
    // The CameraPtr object is a shared pointer, and will generally clean itself
    // up upon exiting its scope.
    //
    // *** LATER ***
    // However, if a shared camera pointer is created in the same scope that a
    // system object is explicitly released (i.e. this scope), the reference to
    // the camera must be broken by manually setting the pointer to nullptr.
    //
    CameraPtr pCam = nullptr;

    // Run example on each camera
    for (unsigned int i = 0; i < numCameras; i++)
    {
        // Select camera
        pCam = camList.GetByIndex(i);

        std::cout <<"hi sasha\n";
        pCam->Init();
        Spinnaker::GenApi::INodeMap& appLayerNodeMap = pCam->GetNodeMap();
        CFloatPtr cam_exp = appLayerNodeMap.GetNode("Gain");
        std::cout << "Gain: " << cam_exp->GetValue();
        pCam->DeInit();

        cout << endl << "Running example for camera " << i << "..." << endl;

        // Run example
        result = result | RunSingleCamera(pCam);

        cout << "Camera " << i << " example complete..." << endl << endl;
    }

    //
    // Release shared pointer reference to camera before releasing system
    //
    // *** NOTES ***
    // Had the CameraPtr object been created within the for-loop, it would not
    // be necessary to manually break its reference because the shared pointer
    // would have automatically cleaned itself up upon exiting the loop.
    //
    pCam = nullptr;

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    system->ReleaseInstance();

    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}
