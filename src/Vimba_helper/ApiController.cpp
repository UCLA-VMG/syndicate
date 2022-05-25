/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.cpp

  Description: Implementation file for the ApiController helper class that
               demonstrates how to implement an asynchronous, continuous image
               acquisition with VimbaCPP.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include "ApiController.h"
#include <sstream>
#include <iostream>
#include "StreamSystemInfo.h"
#include "ErrorCodeToMessage.h"

// #include <chrono>

namespace AVT {
namespace VmbAPI {

// enum    { NUM_FRAMES=3, };

ApiController::ApiController()
    // Get a reference to the Vimba singleton
    : m_system( VimbaSystem::GetInstance() )
{
}

ApiController::~ApiController()
{
}

// Translates Vimba error codes to readable error messages
std::string ApiController::ErrorCodeToMessage( VmbErrorType eErr ) const
{
    return AVT::VmbAPI::ErrorCodeToMessage( eErr );
}

VmbErrorType ApiController::StartUp()
{
    VmbErrorType res;

    // Start Vimba
    res = m_system.Startup();
    if( VmbErrorSuccess == res )
    {
        // This will be wrapped in a shared_ptr so we don't delete it
        SP_SET( m_pCameraObserver , new CameraObserver() );
        // Register an observer whose callback routine gets triggered whenever a camera is plugged in or out
        res = m_system.RegisterCameraListObserver( m_pCameraObserver );
    }

    return res;
}

void ApiController::ShutDown()
{
    // Release Vimba
    m_system.Shutdown();
}
/*** helper function to set image size to a value that is dividable by modulo 2.
\note this is needed because AVTImageTransform does not support odd values for some input formats
*/
inline VmbErrorType SetValueIntMod2( const CameraPtr &camera, const std::string &featureName, VmbInt64_t &storage )
{
    VmbErrorType    res;
    FeaturePtr      pFeature;
    res = SP_ACCESS( camera )->GetFeatureByName( featureName.c_str(), pFeature );
    if( VmbErrorSuccess == res )
    {
        VmbInt64_t minValue,maxValue;
        res = SP_ACCESS( pFeature )->GetRange( minValue,maxValue );
        if( VmbErrorSuccess == res )
        {
            maxValue = ( maxValue>>1 )<<1; // mod 2 dividable
            res = SP_ACCESS( pFeature )->SetValue( maxValue );
            if( VmbErrorSuccess == res )
            {
                storage = maxValue;
            }
        }
    }
    return res;
}

CameraPtr ApiController::getCam(const std::string& rStrCameraID)
{
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID(rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera);
    return m_pCamera;
}

VmbErrorType ApiController::setPixelFormatMono8(CameraPtr m_pCamera)
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


VmbErrorType ApiController::setContinuousAcquisitonMode(CameraPtr m_pCamera)
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

VmbErrorType ApiController::setFps(CameraPtr m_pCamera, float fps)
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

VmbErrorType ApiController::setHeight(CameraPtr m_pCamera, int height)
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

VmbErrorType ApiController::setWidth(CameraPtr m_pCamera, int width)
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

VmbErrorType ApiController::saveBitMap(FramePtr& rpFrame, const char* pFileName)
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
                        std::cout << "Could not create bitmap.\n";
                        err = VmbErrorResources;
                    }
                    else
                    {
                        // Save the bitmap
                        // if (0 == AVTWriteBitmapToFile(&bitmap, pFileName))
                        if (0 == SaveCVMat(&bitmap, pFileName))
                        {
                            std::cout << "Could not write bitmap to file.\n";
                            err = VmbErrorOther;
                        }
                        else
                        {
                            // std::cout << "Bitmap successfully written to file \"" << pFileName << "\"\n";
                            // Release the bitmap's buffer
                            if (0 == AVTReleaseBitmap(&bitmap))
                            {
                                std::cout << "Could not release the bitmap.\n";
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

void ApiController::closeCam(CameraPtr m_pCamera)
{
    // Try to set Mono8
    m_pCamera->Close();
}

VmbErrorType ApiController::StartContinuousImageAcquisition(CameraPtr m_pCamera, const int num_frames )
{
    VmbErrorType    res = VmbErrorSuccess;
    std::string prefix = "nir2_";
    std::string file_ext = ".bmp";
    // std::string file_ext = ".bmp";
    // set camera so that transform algorithms will never fail
    if ( VmbErrorSuccess == res )
    {
        // Set the GeV packet size to the highest possible value
        // (In this example we do not test whether this cam actually is a GigE cam)
        VmbFrameStatusType status = VmbFrameStatusIncomplete;
        // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
        SP_SET( m_pFrameObserver, new FrameObserver( m_pCamera ) );
        // Start streaming
        // res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( NUM_FRAMES,  m_pFrameObserver );
        // auto start = std::chrono::high_resolution_clock::now();
        res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( num_frames,  m_pFrameObserver );
        // set count = 0
        int count = 0;
        // begin recording
        while (VmbErrorSuccess == res && count < num_frames) //  && ApiController::FrameAvailable() )
        {
            while(!(ApiController::FrameAvailable()));
            // Get Frame
            FramePtr pFrame = ApiController::GetFrame();
            res = pFrame->GetReceiveStatus( status );
            // if frame is receievd, save
            if (VmbErrorSuccess == res)
            {
                // create filename
                std::string idx = std::to_string(count);
                std::string file = prefix + idx + file_ext;
                const char* pFileName = file.c_str();
                // save bitmap
                if (VmbErrorSuccess == res)
                {
                    res = ApiController::saveBitMap(pFrame, pFileName);
                    if (VmbErrorSuccess == res)
                    {
                        // exit if can't save bitmaps
                        count++;
                    }
                }
            }
        }
        // auto stop = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        // std::cout << "Difference: " << duration.count() << std::endl << std::endl;
    }
    return res;
}

VmbErrorType ApiController::StopContinuousImageAcquisition()
{
    // Stop streaming
    m_pCamera->StopContinuousImageAcquisition();

    // Close camera
    return  m_pCamera->Close();
}

void ApiController::runContinuous(const std::string& rStrCameraID, const int num_frames)
{
    CameraPtr cam = ApiController::getCam(rStrCameraID);
    VmbErrorType err = ApiController::setPixelFormatMono8(cam);
    if (VmbErrorSuccess == err)
    {
        err = ApiController::setFps(cam, 30.0);
        if (VmbErrorSuccess == err)
        {
            // err = ApiController::setHeight(cam, 640);
            if (VmbErrorSuccess == err)
            {
                // err = ApiController::setWidth(cam, 640);
                if (VmbErrorSuccess == err)
                {
                    err = ApiController::StartContinuousImageAcquisition(cam, num_frames);
                    if (VmbErrorSuccess != err)
                    {
                        ApiController::StopContinuousImageAcquisition();
                        ApiController::closeCam(cam);
                    }
                }
            }
        }
    }
    // if (VmbErrorSuccess == err)
    // {
    //     std::cout << "beginning ApiController::StartContinuousImageAcquisition!" << std::endl << std::endl;
    //     err = ApiController::StartContinuousImageAcquisition(cam, num_frames);
    //     if (VmbErrorSuccess != err)
    //     {
    //         std::cout << "beginning ApiController::StopContinuousImageAcquisition!" << std::endl << std::endl;
    //         ApiController::StopContinuousImageAcquisition();
    //         ApiController::closeCam(cam);
    //     }
    // }
}

CameraPtrVector ApiController::GetCameraList()
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

int ApiController::GetWidth()
{
    return (int)m_nWidth;
}

int ApiController::GetHeight()
{
    return (int)m_nHeight;
}

VmbPixelFormatType ApiController::GetPixelFormat()
{
    return (VmbPixelFormatType)m_nPixelFormat;
}

// Returns the oldest frame that has not been picked up yet
FramePtr ApiController::GetFrame()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver )->GetFrame();
}

// // Get the oldest frame and encode data in the given Mat
// VmbErrorType ApiController::GetFrame(cv::Mat& img_matrix)
// {
//     VmbErrorType err = VmbErrorSuccess;
//     FramePtr frame = SP_DYN_CAST( m_pFrameObserver, FrameObserver )->GetFrame();
//     VmbFrameStatusType status;
//     frame->GetReceiveStatus( status );

//     // See if it is not corrupt
//     if( status == VmbFrameStatusComplete )
//     {
//         unsigned char* buffer;
//         VmbErrorType err = SP_ACCESS( frame )->GetImage( buffer );
        
//         if( err == VmbErrorSuccess )
//         {
//             VmbUint32_t size;
//             err = SP_ACCESS( frame )->GetImageSize( size );
            
//             img_matrix = 


//             // // Copy it
//             // VmbImage            SourceImage,DestImage;
//             // SourceImage.Size    = sizeof( SourceImage );
//             // DestImage.Size      = sizeof( DestImage );
            
//             // VmbSetImageInfoFromPixelFormat( m_nPixelFormat, m_nWidth, m_nHeight, &SourceImage );
//             // VmbSetImageInfoFromPixelFormat( VmbPixelFormatBgr8, m_nWidth, m_nHeight, &DestImage );
            
//             SourceImage.Data    = buffer;
//             DestImage.Data      = m.data;
            
//             VmbImageTransform(&SourceImage, &DestImage, NULL, 0);
//             //memcpy (m.data, buffer, size);
//         }
//     }
//     // done copying the frame, give it back to the camera
//     SP_ACCESS( m_pCamera )->QueueFrame( frame );

//     return err;
// }


// Clears all remaining frames that have not been picked up
void ApiController::ClearFrameQueue()
{
    SP_DYN_CAST( m_pFrameObserver,FrameObserver )->ClearFrameQueue();
}

bool ApiController::FrameAvailable()
{
return SP_DYN_CAST( m_pFrameObserver,FrameObserver )->FrameAvailable();
}

unsigned int ApiController::GetQueueFrameSize()
{
return SP_DYN_CAST( m_pFrameObserver,FrameObserver )->GetQueueFrameSize();
}

// Queues a frame to continue streaming
VmbErrorType ApiController::QueueFrame( FramePtr pFrame )
{
    return SP_ACCESS( m_pCamera )->QueueFrame( pFrame );
}

// Returns the camera observer as QObjects to connect their signals to the view's sots
CameraObserver* ApiController::GetCameraObserver()
{
    return SP_DYN_CAST( m_pCameraObserver, CameraObserver ).get();
}

// Returns the frame observer as QObjects to connect their signals to the view's sots
FrameObserver* ApiController::GetFrameObserver()
{
    return SP_DYN_CAST( m_pFrameObserver, FrameObserver ).get();
}

std::string ApiController::GetVersion() const
{
    std::ostringstream os;
    os << m_system;
    return os.str();
}

}} // namespace AVT::VmbAPI::Examples
