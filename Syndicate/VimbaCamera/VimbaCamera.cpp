#include "VimbaCamera.h"

#include <sstream>
#include <iostream>

using namespace std;
// using namespace AVT;
// using namespace VmbAPI;

using namespace Syndicate;

using namespace AVT;
using namespace VmbAPI;

// check vimba C++ manual page 43 for hardware trigger

//================================================ Constructor and Destructor =================================================

VimbaCamera::VimbaCamera(std::unordered_map<std::string, std::any>& sample_config)
    : Syndicate::Camera(sample_config),
    cameraID(any_cast<string>(sample_config["Camera ID"])),
    m_system( VimbaSystem::GetInstance() )
{
    VmbErrorType err;
    try {
        std::cout << endl << endl << "Configuring " << sensorName << "\n";
        // Start Vimba
        err = m_system.Startup();
        if( VmbErrorSuccess == err )
        {
            // This will be wrapped in a shared_ptr so we don't delete it
            SP_SET( m_pCameraObserver , new CameraObserver() );
            // Register an observer whose callback routine gets triggered whenever a camera is plugged in or out
            err = m_system.RegisterCameraListObserver( m_pCameraObserver );
        }

        // get camera ID
        // string strCameraID = GetCameraID();
        // open camera
        CameraPtr m_pCamera = openCam(cameraID);
        // Configure camera
        err = configure(m_pCamera, fps, height, width);
        if (err != VmbErrorSuccess) {
            std::cout << "Camera configuration for device " << cameraID << " unsuccessful, aborting...";
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    this->setHealthCode(HealthCode::ONLINE);
}

VimbaCamera::~VimbaCamera()
{
    // Deinitialize camera
    m_system.Shutdown();
}

//================================================ Acquire and Save Functions ======================================

// void VimbaCamera::AcquireSave(double seconds, string filename_prefix)
void VimbaCamera::AcquireSave(double seconds, boost::barrier& startBarrier)
{
    // 0. Initialize Starting Variables
    this->setHealthCode(HealthCode::RUNNING);
    int imageCnt = 0;
    int num_frames(seconds*int(fps));
    VmbErrorType    res = VmbErrorSuccess;
    string file_ext = ".bmp";
    VmbFrameStatusType status = VmbFrameStatusIncomplete;
    // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
    SP_SET( m_pFrameObserver, new FrameObserver( m_pCamera ) );

    // 1. Wait Until all other sensors have reached here
    startBarrier.wait();
    auto start = chrono::steady_clock::now();

    // 2.0 Begin acquiring images
    try {        
        // Start streaming
        // auto start = chrono::high_resolution_clock::now();
        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( num_frames,  m_pFrameObserver );
        // begin recording
        while (VmbErrorSuccess == res && imageCnt < num_frames) //  && VimbaCamera::FrameAvailable() )
        {
            while(!(VimbaCamera::FrameAvailable()));
            // 2.1 Retrieve next received image
            FramePtr pFrame = VimbaCamera::GetFrame();
            // 2.2 Save Software System Timestamp
            RecordTimeStamp();
            // 2.3 Ensure image completion:
            res = pFrame->GetReceiveStatus( status );
            // 2.4 If Image is Complete:
            if (VmbErrorSuccess == res)
            {
                // create filename
                string idx = to_string(imageCnt);
                string file = rootPath + sensorName + std::string("_") + idx + file_ext;
                const char* pFileName = file.c_str();
                // save bitmap
                if (VmbErrorSuccess == res)
                {
                    res = saveBitMap(pFrame, pFileName);
                    if (VmbErrorSuccess == res)
                    {
                        imageCnt++;
                    }
                }
            }
        }
        res = VimbaCamera::StopContinuousImageAcquisition();
        if (VmbErrorSuccess == res)
        {
            VimbaCamera::closeCam(m_pCamera);
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    auto end = chrono::steady_clock::now();
    SaveTimeStamps();
    cout <<"Time Taken for " << sensorName << (end-start).count() << "\n";
}

// void VimbaCamera::AcquireSaveBarrier(CameraPtr m_pCamera, double seconds, string filename_prefix, boost::barrier& frameBarrier)
void VimbaCamera::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier)
{
    // 0. Initialize Starting Variables
    this->setHealthCode(HealthCode::RUNNING);
    int imageCnt = 0;
    int num_frames(seconds*int(fps));
    VmbErrorType    res = VmbErrorSuccess;
    string file_ext = ".bmp";
    VmbFrameStatusType status = VmbFrameStatusIncomplete;
    // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
    SP_SET( m_pFrameObserver, new FrameObserver( m_pCamera ) );
    auto start = chrono::steady_clock::now();

    // 1.0 Begin acquiring images
    try {        
        // Start streaming
        // auto start = chrono::high_resolution_clock::now();
        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( num_frames,  m_pFrameObserver );
        // begin recording
        while (VmbErrorSuccess == res && imageCnt < num_frames) //  && VimbaCamera::FrameAvailable() )
        {
            frameBarrier.wait();
            while(!(VimbaCamera::FrameAvailable()));
            // 1.1 Retrieve next received image
            FramePtr pFrame = VimbaCamera::GetFrame();
            // 1.2 Save Software System Timestamp
            RecordTimeStamp();
            // 1.3 Ensure image completion:
            res = pFrame->GetReceiveStatus( status );
            // 1.4 If Image is Complete:
            if (VmbErrorSuccess == res)
            {
                runningBuffer.push(pFrame);
                imageCnt++;
            }
        }
        res = VimbaCamera::StopContinuousImageAcquisition();
        if (VmbErrorSuccess == res)
        {
            VimbaCamera::closeCam(m_pCamera);
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    auto end = chrono::steady_clock::now();
    std::cout <<"Time Taken for acquistion " << sensorName << " " << (end-start).count()/1'000'000'000 << "\n";

    start = std::chrono::steady_clock::now();
    for (unsigned int imageCnt = 0; imageCnt < num_frames; imageCnt++) {
        // create filename
        string idx = to_string(imageCnt);
        string file = rootPath + sensorName + std::string("_") + idx + file_ext;
        const char* pFileName = file.c_str();

        // save bitmap
        res = saveBitMap(std::any_cast<FramePtr&>(runningBuffer.front()), pFileName);
        if (!VmbErrorSuccess == res)
        {
            std::cout << "Error Saving " << sensorName << "Frame Number " << imageCnt << "\n";
        }
        runningBuffer.pop();
    }
    end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for Saving " << sensorName << (end-start).count()/1'000'000'000 << "\n";
    this->setHealthCode(HealthCode::ONLINE);
}

void VimbaCamera::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    // 0. Initialize Starting Variables
    this->setHealthCode(HealthCode::RUNNING);
    int imageCnt = 0;
    int num_frames(seconds*int(fps));
    VmbErrorType    res = VmbErrorSuccess;
    string file_ext = ".bmp";
    VmbFrameStatusType status = VmbFrameStatusIncomplete;
    // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
    SP_SET( m_pFrameObserver, new FrameObserver( m_pCamera ) );
    auto start = chrono::steady_clock::now();

    // 1.0 Begin acquiring images
    try {        
        // Start streaming
        // auto start = chrono::high_resolution_clock::now();
        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( num_frames,  m_pFrameObserver );
        // begin recording
        while (VmbErrorSuccess == res && imageCnt < num_frames) //  && VimbaCamera::FrameAvailable() )
        {
            frameBarrier.wait();
            while(!(VimbaCamera::FrameAvailable()));
            // 1.1 Retrieve next received image
            FramePtr pFrame = VimbaCamera::GetFrame();
            // 1.2 Save Software System Timestamp
            RecordTimeStamp();
            // 1.3 Ensure image completion:
            res = pFrame->GetReceiveStatus( status );
            // 1.4 If Image is Complete:
            if (VmbErrorSuccess == res)
            {
                runningBuffer.push(pFrame);
                imageCnt++;
            }
        }
        res = VimbaCamera::StopContinuousImageAcquisition();
        if (VmbErrorSuccess == res)
        {
            VimbaCamera::closeCam(m_pCamera);
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    auto end = chrono::steady_clock::now();
    std::cout <<"Time Taken for acquistion " << sensorName << " " << (end-start).count()/1'000'000'000 << "\n";
    this->setHealthCode(HealthCode::ONLINE);
}

void VimbaCamera::ConcurrentSave()
{
    bool started(false);
    int imgCount = 0;
    VmbErrorType    res = VmbErrorSuccess;
    string file_ext = ".bmp";
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
                string idx = to_string(imgCount);
                string file = rootPath + sensorName + idx + file_ext;
                const char* pFileName = file.c_str();

                // save bitmap
                res = saveBitMap(std::any_cast<FramePtr&>(runningBuffer.front()), pFileName);
                if (!VmbErrorSuccess == res)
                {
                    std::cout << "Error Saving " << sensorName << "Frame Number " << imgCount << "\n";
                }
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

//================================================ Set Functions ====================================================

VmbErrorType VimbaCamera::setPixelFormatMono8(CameraPtr m_pCamera)
{
    // Try to set Mono8
    FeaturePtr pFormatFeature;
    VmbErrorType res = m_pCamera->GetFeatureByName("PixelFormat", pFormatFeature);
    if (VmbErrorSuccess == res)
    {
        res = pFormatFeature->SetValue(VmbPixelFormatMono8);
    }
    return res;
}

VmbErrorType setContinuousAcquisitonMode(CameraPtr m_pCamera)
{
   // Try to set setContinuousAcquisitonMode
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("AcquisitionMode", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       res = pFormatFeature->SetValue("Continuous");
   }
   return res;
}

VmbErrorType VimbaCamera::setFps(CameraPtr m_pCamera, double fps)
{
   // set AcquisitionFrameRateEnable to true
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("AcquisitionFrameRateEnable", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       res = pFormatFeature->SetValue(true);
   }
   
   // Set fps
   FeaturePtr pFormatFeature2;
   // VmbErrorType res = m_pCamera->GetFeatureByName("AcquisitionFrameRate", pFormatFeature2);
   res = m_pCamera->GetFeatureByName("AcquisitionFrameRate", pFormatFeature2);
   if (VmbErrorSuccess == res)
   {
       res = pFormatFeature2->SetValue(fps);
   }
   return res;
}

VmbErrorType VimbaCamera::setHorizontalBinningMethod(CameraPtr m_pCamera)
{
   // Try to set Height
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("BinningHorizontalMode", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       
       res = pFormatFeature->SetValue("Average");
    //    pFormatFeature->GetValue(height); // this should be continuous
   }
   return res;
}

VmbErrorType VimbaCamera::setVerticalBinningMethod(CameraPtr m_pCamera)
{
   // Try to set Height
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("BinningVerticalMode", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       
       res = pFormatFeature->SetValue("Average");
    //    pFormatFeature->GetValue(height); // this should be continuous
   }
   return res;
}

VmbErrorType VimbaCamera::setHorizontalBinningValue(CameraPtr m_pCamera, int binning_value)
{
   // Try to set binning_value
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("BinningHorizontal", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       
       res = pFormatFeature->SetValue(binning_value);
    //    pFormatFeature->GetValue(binning_value); // this should be continuous
   }
   return res;
}

VmbErrorType VimbaCamera::setVerticalBinningValue(CameraPtr m_pCamera, int binning_value)
{
   // Try to set binning_value
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("BinningVertical", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       
       res = pFormatFeature->SetValue(binning_value);
    //    pFormatFeature->GetValue(binning_value); // this should be continuous
   }
   return res;
}


VmbErrorType VimbaCamera::setHeight(CameraPtr m_pCamera, int height)
{
   // Try to set Height
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("Height", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       
       res = pFormatFeature->SetValue(height);
    //    pFormatFeature->GetValue(height); // this should be continuous
   }
   return res;
}

VmbErrorType VimbaCamera::setWidth(CameraPtr m_pCamera, int width)
{
   // Try to set Width
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("Width", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       res = pFormatFeature->SetValue(width);
   }
   return res;
}

VmbErrorType VimbaCamera::setExposure(CameraPtr m_pCamera, double exposure_time_abs)
{
   // Try to set Width
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("ExposureTime", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       res = pFormatFeature->SetValue(exposure_time_abs);
   }
   return res;
}

VmbErrorType VimbaCamera::setGamma(CameraPtr m_pCamera, double gamma)
{
   // Try to set Width
   FeaturePtr pFormatFeature;
   VmbErrorType res = m_pCamera->GetFeatureByName("Gamma", pFormatFeature);
   if (VmbErrorSuccess == res)
   {
       res = pFormatFeature->SetValue(gamma);
   }
   return res;
}

//================================================ Get Functions ====================================================

CameraPtrVector VimbaCamera::GetCameraList()
{
    CameraPtrVector cameras;
    // Get all known cameras
    if( VmbErrorSuccess == m_system.GetCameras( cameras ) )
    {
        // And return them
        return cameras;
    }
    return CameraPtrVector();
}

string VimbaCamera::GetCameraID()
{
    VmbErrorType err;
    char * pCameraID = NULL; 
    string strCameraID;
    if ( NULL == pCameraID )
    {
        CameraPtrVector cameras = GetCameraList();
        if ( cameras.size() <= 0 )
        {
            err = VmbErrorNotFound;
        }
        else
        {
            err = cameras[0]->GetID( strCameraID );
        }
    }
    else
    {
        strCameraID = pCameraID;
    }
    
    if (VmbErrorSuccess == err)
    {
        cout << "Camera ID:" << strCameraID.c_str() << "\n\n";
    }
    return strCameraID;
}

int VimbaCamera::GetWidth()
{
    return (int)m_nWidth;
}

int VimbaCamera::GetHeight()
{
    return (int)m_nHeight;
}

VmbPixelFormatType VimbaCamera::GetPixelFormat()
{
    return (VmbPixelFormatType)m_nPixelFormat;
}

// Returns the oldest frame that has not been picked up yet
FramePtr VimbaCamera::GetFrame()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver )->GetFrame();
}

// Returns the camera observer as QObjects to connect their signals to the view's sots
CameraObserver* VimbaCamera::GetCameraObserver()
{
    return SP_DYN_CAST( m_pCameraObserver, CameraObserver ).get();
}

// Returns the frame observer as QObjects to connect their signals to the view's sots
FrameObserver* VimbaCamera::GetFrameObserver()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver ).get();
}

//================================================ Helper Functions =================================================

VmbErrorType VimbaCamera::saveBitMap(FramePtr& rpFrame, const char* pFileName)
{
    // save single frame
    VmbUint32_t nImageSize = 0;
    VmbErrorType err = rpFrame->GetImageSize(nImageSize);
    if (VmbErrorSuccess == err)
    {
        VmbUint32_t nWidth = 0;
        err = rpFrame->GetWidth(nWidth);
        if (VmbErrorSuccess == err)
        {
            VmbUint32_t nHeight = 0;
            err = rpFrame->GetHeight(nHeight);
            if (VmbErrorSuccess == err)
            {
                VmbUchar_t* pImage = NULL;
                err = rpFrame->GetImage(pImage);
                if (VmbErrorSuccess == err)
                {
                    AVTBitmap bitmap;
                    bitmap.colorCode = ColorCodeMono8;
                    bitmap.bufferSize = nImageSize;
                    bitmap.width = nWidth;
                    bitmap.height = nHeight;
                    // Create the bitmap
                    if (0 == AVTCreateBitmap(&bitmap, pImage))
                    {
                        cout << "Could not create bitmap.\n";
                        err = VmbErrorResources;
                    }
                    else
                    {
                        // Save the bitmap
                        if (0 == AVTWriteBitmapToFile(&bitmap, pFileName))
                        // if (0 == SaveCVMat(&bitmap, pFileName))
                        {
                            cout << "Could not write bitmap to file.\n";
                            err = VmbErrorOther;
                        }
                        else
                        {
                            // cout << "Bitmap successfully written to file \"" << pFileName << "\"\n";
                            // Release the bitmap's buffer
                            if (0 == AVTReleaseBitmap(&bitmap))
                            {
                                cout << "Could not release the bitmap.\n";
                                err = VmbErrorInternalFault;
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}

// This function acquires and saves 10 images from a device.
// bool VimbaCamera::AcquireImages(CameraPtr m_pCamera, const int num_frames, string filename_prefix)
bool VimbaCamera::AcquireImages(CameraPtr m_pCamera, const int num_frames)
{
    bool result = true;
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        VmbErrorType    res = VmbErrorSuccess;
        string filename_prefix = "nir2_";
        string file_ext = ".bmp";
        VmbFrameStatusType status = VmbFrameStatusIncomplete;
        // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
        SP_SET( m_pFrameObserver, new FrameObserver( m_pCamera ) );
        // Start streaming
        // auto start = chrono::high_resolution_clock::now();
        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( num_frames,  m_pFrameObserver );
        int count = 0;
        // begin recording
        while (VmbErrorSuccess == res && count < num_frames) //  && VimbaCamera::FrameAvailable() )
        {
            while(!(FrameAvailable()));
            // Get Frame
            FramePtr pFrame = GetFrame();
            res = pFrame->GetReceiveStatus( status );
            // if frame is received, save
            if (VmbErrorSuccess == res)
            {
                // create filename
                string idx = to_string(count);
                string file = filename_prefix + idx + file_ext;
                const char* pFileName = file.c_str();
                // save bitmap
                if (VmbErrorSuccess == res)
                {
                    res = saveBitMap(pFrame, pFileName);
                    if (VmbErrorSuccess == res)
                    {
                        // exit if can't save bitmaps
                        count++;
                    }
                }
            }
        }
        res = VimbaCamera::StopContinuousImageAcquisition();
        if (VmbErrorSuccess == res)
        {
            VimbaCamera::closeCam(m_pCamera);
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
        return false;
    }
    return result;
}

// VmbErrorType VimbaCamera::configure(CameraPtr m_pCamera, double fps, int height, int width) {
//     VmbErrorType err = this->VimbaCamera::setPixelFormatMono8(m_pCamera);
//     if (VmbErrorSuccess == err)
//     {
//         err = setFps(m_pCamera, fps);
//         if (VmbErrorSuccess == err)
//         {
//             err = setHeight(m_pCamera, height);
//             if (VmbErrorSuccess == err)
//             {
//                 err = setWidth(m_pCamera, width);
//                 if (VmbErrorSuccess == err)
//                 {
//                     err = setExposure(m_pCamera, 33000.0);
//                     if (VmbErrorSuccess == err)
//                     {
//                         err = setGamma(m_pCamera, 0.45);
//                     }
//                 }
//             }
//         }
//     }
//     return err;
// }

VmbErrorType VimbaCamera::configure(CameraPtr m_pCamera, double fps, int height, int width) {
    VmbErrorType err = this->VimbaCamera::setPixelFormatMono8(m_pCamera);
    if (VmbErrorSuccess == err)
    {
        err = setFps(m_pCamera, fps);
        if (VmbErrorSuccess == err)
        {
            err = setHorizontalBinningMethod(m_pCamera);
            if (VmbErrorSuccess == err)
            {
                err = setVerticalBinningMethod(m_pCamera);
                if (VmbErrorSuccess == err)
                {
                    err = setHorizontalBinningValue(m_pCamera, 4);
                    if (VmbErrorSuccess == err)
                    {
                        err = setVerticalBinningValue(m_pCamera, 4);
                        if (VmbErrorSuccess == err)
                        {
                            err = setExposure(m_pCamera, 33000.0);
                            if (VmbErrorSuccess == err)
                            {
                                err = setGamma(m_pCamera, 0.45);
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}

bool VimbaCamera::FrameAvailable()
{
return SP_DYN_CAST( m_pFrameObserver,FrameObserver )->FrameAvailable();
}

// Translates Vimba error codes to readable error messages
string ErrorCodeToMessage( VmbErrorType eErr )
{
    return ErrorCodeToMessage( eErr );
}

void VimbaCamera::closeCam(CameraPtr m_pCamera)
{
    // Close Camera
    m_pCamera->Close();
}

VmbErrorType VimbaCamera::StopContinuousImageAcquisition()
{
    // Stop streaming
    m_pCamera->StopContinuousImageAcquisition();

    // Close camera
    return  m_pCamera->Close();
}

CameraPtr VimbaCamera::openCam(const string& rStrCameraID)
{
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID(rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera);
    return m_pCamera;
}

// }}