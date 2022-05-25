/*=============================================================================
  Copyright (C) 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.cpp

  Description: Implementation of main entry point of SynchronousGrabConsole
               example of VimbaCPP.

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

#include <string>
#include <cstring>
#include <iostream>

#include "ApiController.h"

unsigned char StartsWith(const char *pString, const char *pStart)
{
    if(NULL == pString)
    {
        return 0;
    }
    if(NULL == pStart)
    {
        return 0;
    }

    if(std::strlen(pString) < std::strlen(pStart))
    {
        return 0;
    }

    if(std::memcmp(pString, pStart, std::strlen(pStart)) != 0)
    {
        return 0;
    }

    return 1;
}

int main( int argc, char* argv[] )
{
    VmbErrorType    err         = VmbErrorSuccess;

    char *          pCameraID   = NULL;             // The ID of the camera to use
    const char *    pFileName   = NULL;             // The filename for the bitmap to save
    bool            bPrintHelp  = false;            // Output help?
    int             i;                              // Counter for some iteration
    char *          pParameter;                     // The command line parameter
    const int       num_frames  = 300;               // The Number of frames to record 

    // std::cout << "//////////////////////////////////////////\n";
    // std::cout << "/// Vimba API Synchronous Grab Example ///\n";
    // std::cout << "//////////////////////////////////////////\n\n";

    //////////////////////
    //Parse command line//
    //////////////////////

    for( i = 1; i < argc; ++i )
    {
        pParameter = argv[i];
        if( 0 > std::strlen( pParameter ))
        {
            err = VmbErrorBadParameter;
            break;
        }

        if( '/' == pParameter[0] )
        {
            if( StartsWith( pParameter, "/f:" ))
            {
                if( NULL != pFileName )
                {
                    err = VmbErrorBadParameter;
                    break;
                }

                pFileName = pParameter + 3;
                if( 0 >= std::strlen( pFileName ))
                {
                    err = VmbErrorBadParameter;
                    break;
                }
            }
            else if( 0 == std::strcmp( pParameter, "/h" ))
            {
                if(     ( NULL != pCameraID )
                    ||  ( NULL != pFileName )
                    ||  ( bPrintHelp ))
                {
                    err = VmbErrorBadParameter;
                    break;
                }

                bPrintHelp = true;
            }
            else
            {
                err = VmbErrorBadParameter;
                break;
            }
        }
        else
        {
            if( NULL != pCameraID )
            {
                err = VmbErrorBadParameter;
                break;
            }

            pCameraID = pParameter;
        }
    }

    //Write out an error if we could not parse the command line
    if ( VmbErrorBadParameter == err )
    {
        std::cout << "Invalid parameters!\n\n";
        bPrintHelp = true;
    }

    //Print out help and end program
    if ( bPrintHelp )
    {
        std::cout << "Usage: SynchronousGrab [CameraID] [/h] [/f:FileName]\n";
        std::cout << "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)\n";
        std::cout << "              /h          Print out help\n";
        std::cout << "              /f:FileName File name for operation\n";
        std::cout << "                          (default \"SynchronousGrab.bmp\" if not specified)\n";
    }
    else
    {
        if ( NULL == pFileName )
        {
            pFileName = "SynchronousGrab.bmp";
        }

        AVT::VmbAPI::ApiController apiController;
        
        // std::cout << "Vimba C++ API Version " << apiController.GetVersion() << "\n";

        err = apiController.StartUp();
        if ( VmbErrorSuccess == err )
        {
            std::string strCameraID;
            if ( NULL == pCameraID )
            {
                AVT::VmbAPI::CameraPtrVector cameras = apiController.GetCameraList();
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
                std::cout << "Camera ID:" << strCameraID.c_str() << "\n\n";

                ///////////
                apiController.runContinuous(strCameraID, num_frames);
                ///////////
            }

            apiController.ShutDown();
        }

        if ( VmbErrorSuccess != err )
        {
            std::string strError = apiController.ErrorCodeToMessage( err );
            std::cout << "\nAn error occurred: " << strError.c_str() << "\n";
        }
    }

    // getchar();
    return err;
}
