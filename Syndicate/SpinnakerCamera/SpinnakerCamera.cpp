#include "SpinnakerCamera.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

using namespace Syndicate;

SpinnakerCamera::SpinnakerCamera(ptree::value_type& tree, std::string& savePath)
    : Syndicate::Camera(tree, savePath), 
    cameraID(tree.second.get<std::string>("camera_id")),
    pixelFormat(tree.second.get<std::string>("pixel_format"))
{
    // Retrieve singleton reference to system object
    system = System::GetInstance();
    // Retrieve list of cameras from the system
    camList = system->GetCameras();
    // Retrieve camera.
    flir_cam = getCam(system, camList, cameraID);

    // Initialize camera
    flir_cam->Init();
    // Get NodeMap and nodeMapTLDevice
    INodeMap& nodeMap = flir_cam->GetNodeMap();
    INodeMap& nodeMapTLDevice = flir_cam->GetTLDeviceNodeMap();

    // Configure camera with specified fps/height/width
    try {
        std::cout << "I am the " << sensorName << "\n";
        if (!configure(flir_cam, nodeMap, fps, height, width, cameraType)) {
            std::cout << "Camera configuration for device " << cameraID << " unsuccessful, aborting...";
        }
    }
    catch (Spinnaker::Exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    // Enable Hardware Synchronization if asked for
    if(hardwareSync)
    {
        if(primary)
        {
            setPrimary(nodeMap, cameraType);
        }
        if(!primary)
        {
            setSecondary(nodeMap);
        }
    }

    this->setHealthCode(HealthCode::ONLINE);
}

SpinnakerCamera::~SpinnakerCamera()
{
    // Deinitialize camera
    flir_cam->DeInit();
}

void SpinnakerCamera::AcquireSave(double seconds, boost::barrier& startBarrier)
{
    // 0. Initialize Starting Variables
    this->setHealthCode(HealthCode::RUNNING);
    int result = 0;
    int num_frames(seconds*fps);
    ImagePtr convertedImage = nullptr;
    
    // 1. Wait Until all other sensors have reached here
    startBarrier.wait();

    auto start = std::chrono::steady_clock::now();
    // 2.0 Begin acquiring images
    flir_cam->BeginAcquisition();
    const unsigned int k_numImages = num_frames;
    for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) 
    {
        try 
        {
            // 2.1 Retrieve next received image
            ImagePtr pResultImage = flir_cam->GetNextImage(1000);
            // 2.2 Save Software System Timestamp
            RecordTimeStamp();
            // 2.3 Ensure image completion:
            if (pResultImage->IsIncomplete()) 
            {
                // Retrieve and print the image status description
                logFile << "Image incomplete: " 
                        << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                        << "..." << endl
                        << endl;
            }
            // 2.4 If Image is Complete:
            else 
            {
                if(pixelFormat == "Mono"){
                    convertedImage = pResultImage->Convert(PixelFormat_Mono8, NO_COLOR_PROCESSING);
                }
                else if(pixelFormat == "RGB"){
                    convertedImage = pResultImage->Convert(PixelFormat_BGR8, NO_COLOR_PROCESSING);
                }
                else
                    std::cout << "Error: Pixel Format Invalid. \n";
                // runningBuffer.push(convertedImage);
                ostringstream filename;
                filename << rootPath << sensorName << "_" << imageCnt;
                convertedImage->Save(filename.str().c_str(),  Spinnaker::ImageFileFormat::BMP);
                logFile << sensorName << " " << pResultImage->GetFrameID() << std::endl;
            }
            // Release image
            pResultImage->Release();
            // std::cout << endl;
        }
        catch (Spinnaker::Exception& e) {
            std::cout << "Error: " << e.what() << endl;
            result = -1;
        }
    }
    // End acquisition
    flir_cam->EndAcquisition();
    auto end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for acquistion " << sensorName << " " << static_cast<double>((end-start).count())/1'000'000'000 << "\n";
    logFile << "Time Taken for acquistion " << sensorName << " " << static_cast<double>((end-start).count())/1'000'000'000 << "\n";
    SaveTimeStamps();

    this->setHealthCode(HealthCode::ONLINE);
}

void SpinnakerCamera::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier)
{
    this->setHealthCode(HealthCode::RUNNING);
    int result = 0;
    const string deviceSerialNumber = GetDeviceSerial(flir_cam);
    int num_frames(seconds*fps);
    std::cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    auto start = std::chrono::steady_clock::now();
    ImagePtr convertedImage = nullptr;
    // Begin acquiring images
    flir_cam->BeginAcquisition();
    std::cout << "Acquiring images..." << endl << endl;
    // Retrieve, convert, and save images
    const unsigned int k_numImages = num_frames;
    for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
        frameBarrier.wait();
        try {
            // Retrieve next received image
            ImagePtr pResultImage = flir_cam->GetNextImage(1000);
            // Ensure image completion
            if (pResultImage->IsIncomplete()) {
                // Retrieve and print the image status description
                std::cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                        << "..." << endl
                        << endl;
            }
            else {
                if(pixelFormat == "Mono"){
                    convertedImage = pResultImage->Convert(PixelFormat_Mono8, NO_COLOR_PROCESSING);
                }
                else if(pixelFormat == "RGB"){
                    convertedImage = pResultImage->Convert(PixelFormat_BGR8, NO_COLOR_PROCESSING);
                }
                else
                    std::cout << "Error: Pixel Format Invalid. \n";
                runningBuffer.push(convertedImage);
                std::cout << sensorName << pResultImage->GetFrameID() << "\n";
            }
            // Release image
            pResultImage->Release();
            // std::cout << endl;
        }
        catch (Spinnaker::Exception& e) {
            std::cout << "Error: " << e.what() << endl;
            result = -1;
        }
    }
    // End acquisition
    flir_cam->EndAcquisition();
    auto end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for acquistion " << sensorName << " " << (end-start).count()/1'000'000'000 << "\n";

    start = std::chrono::steady_clock::now();
    for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
            // Create a unique filename
        ostringstream filename;
        filename << rootPath << sensorName << "_" << imageCnt;
        std::any_cast<ImagePtr>(runningBuffer.front())->Save(filename.str().c_str(),  Spinnaker::ImageFileFormat::BMP);
        runningBuffer.pop();
    }
    end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for Saving " << sensorName << (end-start).count()/1'000'000'000 << "\n";
    this->setHealthCode(HealthCode::ONLINE);
}

void SpinnakerCamera::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    this->setHealthCode(HealthCode::RUNNING);
    int result = 0;
    const string deviceSerialNumber = GetDeviceSerial(flir_cam);
    int num_frames(seconds*fps);
    std::cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    auto start = std::chrono::steady_clock::now();
    ImagePtr convertedImage = nullptr;
    // Begin acquiring images
    flir_cam->BeginAcquisition();
    std::cout << "Acquiring images..." << endl << endl;
    // Retrieve, convert, and save images
    const unsigned int k_numImages = num_frames;
    for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
        frameBarrier.wait();
        try {
            // Retrieve next received image
            ImagePtr pResultImage = flir_cam->GetNextImage(1000);
            // Ensure image completion
            if (pResultImage->IsIncomplete()) {
                // Retrieve and print the image status description
                std::cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                        << "..." << endl
                        << endl;
            }
            else {
                if(pixelFormat == "Mono"){
                    convertedImage = pResultImage->Convert(PixelFormat_Mono8, NO_COLOR_PROCESSING);
                }
                else if(pixelFormat == "RGB"){
                    convertedImage = pResultImage->Convert(PixelFormat_BGR8, NO_COLOR_PROCESSING);
                }
                else
                    std::cout << "Error: Pixel Format Invalid. \n";
                // mtx_.lock();
                runningBuffer.push(convertedImage);
                // mtx_.unlock();
                std::cout << sensorName << pResultImage->GetFrameID() << "\n";
            }
            // Release image
            pResultImage->Release();
            // std::cout << endl;
        }
        catch (Spinnaker::Exception& e) {
            std::cout << "Error: " << e.what() << endl;
            result = -1;
        }
    }
    // End acquisition
    flir_cam->EndAcquisition();
    auto end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for acquistion " << sensorName << " " << static_cast<float>((end-start).count())/1'000'000'000 << "\n";
    this->setHealthCode(HealthCode::ONLINE);
    
}

void SpinnakerCamera::ConcurrentSave()
{
    bool started(false);
    int imgCount = 0;
    auto start = std::chrono::steady_clock::now();
    for(;;)
    {
        if(this->checkHealthCode() == HealthCode::RUNNING || !runningBuffer.empty())
        {
            started=true;
            // mtx_.lock();
            if(runningBuffer.empty())
            {
                // mtx_.unlock();
                std::cout << sensorName << ": I am empty!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(10*static_cast<int>(1000/fps)));
            }
            else
            {
                // Create a unique filename
                ostringstream filename;
                filename << rootPath << sensorName << "_" << imgCount;
                std::any_cast<ImagePtr>(runningBuffer.front())->Save(filename.str().c_str(),  Spinnaker::ImageFileFormat::BMP);
                runningBuffer.pop();
                // std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000/fps/30)));
                // mtx_.unlock();
                ++imgCount;
            }

        }
        else if(!started && this->checkHealthCode() == HealthCode::ONLINE) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else if(started && this->checkHealthCode() == HealthCode::ONLINE)
        {
            break;
        }
        else
        {
            std::cout << "Something is very wrong!\n";
            std::this_thread::sleep_for (std::chrono::seconds(1));
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for Saving " << sensorName << " " << static_cast<float>((end-start).count())/1'000'000'000 << "\n";


}


// This helper function retrieves the device serial number from the cameras nodemap.
std::string GetDeviceSerial(Spinnaker::CameraPtr pCam) {
    INodeMap& nodeMap = pCam->GetTLDeviceNodeMap();
    CStringPtr ptrDeviceSerialNumber = nodeMap.GetNode("DeviceSerialNumber");
    if ((IsAvailable(ptrDeviceSerialNumber)) && (IsReadable(ptrDeviceSerialNumber))) {
        std::cout << string(ptrDeviceSerialNumber->GetValue()) << endl;
        return string(ptrDeviceSerialNumber->GetValue());
    }
    return "";
}

bool setPrimary(INodeMap& nodeMap, std::string& cameraName)
{
    if(cameraName == "BackflyS")
    {
        std::cout << "***********\n";
        CEnumerationPtr ptrLine = nodeMap.GetNode("LineSelector");
        CEnumEntryPtr ptrLine1 = ptrLine->GetEntryByName("Line1");
        ptrLine->SetIntValue(ptrLine1->GetValue());

        CEnumerationPtr ptrLineMode = nodeMap.GetNode("LineMode");
        CEnumEntryPtr ptrLineModeVal = ptrLineMode->GetEntryByName("Output");
        ptrLineMode->SetIntValue(ptrLineModeVal->GetValue());

        CEnumEntryPtr ptrLine2 = ptrLine->GetEntryByName("Line2");
        ptrLine->SetIntValue(ptrLine2->GetValue());

        CBooleanPtr ptr3_3V = nodeMap.GetNode("V3_3Enable");
        ptr3_3V->SetValue(true);
        std::cout << "****done*******\n";





        return true;
    }
    else
    {
        return false;
    }
    return false;
}

bool setSecondary(INodeMap& nodeMap)
{
    std::cout << "Setting sec 1\n";
    CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
    CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
    ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

    CEnumerationPtr ptrTriggerSelector = nodeMap.GetNode("TriggerSelector");
    CEnumEntryPtr ptrTriggerSelectorFrameStart = ptrTriggerSelector->GetEntryByName("FrameStart");
    ptrTriggerSelector->SetIntValue(ptrTriggerSelectorFrameStart->GetValue());

    CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
    CEnumEntryPtr ptrTriggerSourceHardware = ptrTriggerSource->GetEntryByName("Line3");
    ptrTriggerSource->SetIntValue(ptrTriggerSourceHardware->GetValue());

    CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
    ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());

    CEnumerationPtr ptrTriggerOverlap = nodeMap.GetNode("TriggerOverlap");
    CEnumEntryPtr ptrTriggerOverlapReadOut = ptrTriggerOverlap->GetEntryByName("ReadOut");
    ptrTriggerOverlap->SetIntValue(ptrTriggerOverlapReadOut->GetValue());
    std::cout << "Setting sec 2\n";


    return true;
}

bool setResolution(INodeMap& nodeMap, int height, int width) {
    bool result = true;
    try {
        // Set height
        CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
        if(!IsWritable(ptrHeight->GetAccessMode()) || !IsReadable(ptrHeight->GetAccessMode()) || ptrHeight->GetInc() == 0 || ptrHeight->GetMax() == 0) {
            std::cout << "Unable to read or write to Height. Aborting..." << endl;
            return false;
        }
        ptrHeight->SetValue(height);
        int heightToSet = static_cast<int>(ptrHeight->GetValue());
        std::cout << "Height: " << heightToSet << endl << endl;

        // Set width
        CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
        if(!IsWritable(ptrWidth->GetAccessMode()) || !IsReadable(ptrWidth->GetAccessMode()) || ptrWidth->GetInc() == 0 || ptrWidth->GetMax() == 0) {
            std::cout << "Unable to read or write to Width. Aborting..." << endl;
            return false;
        }
        ptrWidth->SetValue(width);
        int widthToSet = static_cast<int>(ptrWidth->GetValue());
        std::cout << "Width: " << widthToSet << endl << endl;
    }
    catch (Exception& e) {
        std::cout << "Error configuring resolution: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool setFps(INodeMap& nodeMap, float fps) {
    bool result = true;

    CFloatPtr acquisition_frame_rate = nodeMap.GetNode("AcquisitionFrameRate");
    float frameRateToSet = static_cast<float>(acquisition_frame_rate->GetValue());
    cout << "Something -  " << frameRateToSet << "..." << endl;

    try {

        try {
            // Set frame_rate_auto_node to false
            CEnumerationPtr frame_rate_auto_node = nodeMap.GetNode("AcquisitionFrameRateAuto");
            CEnumEntryPtr node_frame_rate_auto_off = frame_rate_auto_node->GetEntryByName("Off");
            const int64_t frame_rate_auto_off = node_frame_rate_auto_off->GetValue();
            frame_rate_auto_node->SetIntValue(frame_rate_auto_off);
        }
        catch (Exception& e) {std::cout << "AcquisitionFrameRateAuto Unable to Turn off. \n";}

        // Set frame_rate_enable to true
        CBooleanPtr enable_rate_mode = nodeMap.GetNode("AcquisitionFrameRateEnable");
        if (!IsWritable(enable_rate_mode)) {
            cout << "enable_rate_mode not writable. Aborting..." << endl << endl;
            // return false;
        }
        try {
            enable_rate_mode->SetValue(true);
        }
        catch(Exception& e) {
            std::cerr << e.what() << '\n'; // "Could not enable frame rate: {0}".format(ex)
        }
        // CBooleanPtr acquisition_frame_rate_enable = nodeMap.GetNode("AcquisitionFrameRateEnable");

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

    acquisition_frame_rate = nodeMap.GetNode("AcquisitionFrameRate");
    frameRateToSet = static_cast<float>(acquisition_frame_rate->GetValue());
    cout << "Something -  " << frameRateToSet << "..." << endl;

    return result;
}

bool setContinuousAcquisitionMode(INodeMap& nodeMap) {
    bool result = true;
    try {
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsWritable(ptrAcquisitionMode)) {
            std::cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
            return false;
        }
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsReadable(ptrAcquisitionModeContinuous)) {
            std::cout << "Unable to set acquisition mode to continuous (enum entry retrieval). Aborting..." << endl << endl;
            return false;
        }
        const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
        std::cout << "ptrAcquisitionMode: "<< ptrAcquisitionMode->GetEntryByName("Continuous") << endl;
    }
    catch (Exception& e) {
        std::cout << "Error configuring fps: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool configure(CameraPtr pCam, INodeMap& nodeMap, float fps = 30.0, int height = 640, int width = 640, std::string cameraType = "None") {
    bool result = true;
    try {
        // Get the device serial number
        const string deviceSerialNumber = GetDeviceSerial(pCam);
        // Add a new entry into the camera grab info map using the default GrabInfo constructor
        std::cout << "Configuring device " << deviceSerialNumber << endl;

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
        std::cout << "Error configuring camera: " << e.what() << endl;
        result = false;
    }
    return result;
}

// This function acquires and saves 10 images from a device.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, int num_frames) {
    int result = 0;
    const string deviceSerialNumber = GetDeviceSerial(pCam);
    std::cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        // Begin acquiring images
        pCam->BeginAcquisition();
        std::cout << "Acquiring images..." << endl << endl;
        // Retrieve, convert, and save images
        const unsigned int k_numImages = num_frames;
        for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++) {
            try {
                // Retrieve next received image
                ImagePtr pResultImage = pCam->GetNextImage(1000);
                // Ensure image completion
                if (pResultImage->IsIncomplete()) {
                    // Retrieve and print the image status description
                    std::cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
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
                    // std::cout << "Image saved at " << filename.str() << endl;
                }
                // Release image
                pResultImage->Release();
                std::cout << endl;
            }
            catch (Spinnaker::Exception& e) {
                std::cout << "Error: " << e.what() << endl;
                result = -1;
            }
        }
        // End acquisition
        pCam->EndAcquisition();
    }
    catch (Spinnaker::Exception& e) {
        std::cout << "Error: " << e.what() << endl;
        return -1;
    }
    return result;
}

CameraPtr getCam(SystemPtr system, CameraList camList, string serialNumber) {
    unsigned int numCameras = camList.GetSize();
    std::cout << "Number of cameras detected: " << numCameras << endl << endl;
    // Finish if there are no cameras
    if (numCameras == 0) {
        camList.Clear();
        system->ReleaseInstance();
        std::cout << "Not enough cameras!" << endl;
    }
    // Configure nir connected camera to use User Set 1 and register image events
    for (unsigned int camIndex = 0; camIndex < camList.GetSize(); camIndex++) {
        CameraPtr cam = camList[camIndex];
        const string deviceSerialNumber = GetDeviceSerial(cam);
        if(string(deviceSerialNumber) == serialNumber) {
            std::cout << "Found!" << endl;
            return cam;
        }
    }
    return nullptr;
}