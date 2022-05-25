#include "Vimba.h"

namespace AVT {
namespace VmbAPI {

// Constructor for the FrameObserver class
FrameObserver::FrameObserver(CameraPtr pCamera) : IFrameObserver(pCamera){}

// Frame callback notifies about incoming frames
void FrameObserver::FrameReceived(const FramePtr pFrame)
{
// Send notification to working thread
// Do not apply image processing within this callback (performance)
// When the frame has been processed , requeue it
m_pCamera ->QueueFrame(pFrame);
}

void Vimba::RunExample(void)
{
    VmbInt64_t nPLS; // Payload size value
    FeaturePtr pFeature; // Generic feature pointer
    VimbaSystem &sys = VimbaSystem::GetInstance(); // Create and get Vimba singleton
    CameraPtrVector cameras; // Holds camera handles
    CameraPtr camera;
    FramePtrVector frames(15); // Frame array

    // Start the API, get and open cameras
    sys.Startup();
    sys.GetCameras(cameras);
    camera = cameras[0];
    camera ->Open(VmbAccessModeFull);

    // Get the image size for the required buffer
    // Allocate memory for frame buffer
    // Register frame observer/callback for each frame
    // Announce frame to the API
    camera ->GetFeatureByName("PayloadSize", pFeature);
    pFeature ->GetValue(nPLS);
    for(FramePtrVector::iterator iter=frames.begin(); frames.end()!=iter; ++iter)
    {
        (*iter).reset(new Frame(nPLS));
        (*iter)->RegisterObserver(IFrameObserverPtr(new FrameObserver(camera)));
        camera ->AnnounceFrame(*iter);
    }

    // Start the capture engine (API)
    camera ->StartCapture();
    for(FramePtrVector::iterator iter=frames.begin(); frames.end()!=iter; ++iter)
    {

        // Put frame into the frame queue
        camera ->QueueFrame(*iter);
    }
    
    // Start the acquisition engine (camera)
    camera ->GetFeatureByName("AcquisitionStart", pFeature);
    pFeature ->RunCommand();
    
    // Program runtime , e.g., Sleep(2000);

    // Stop the acquisition engine (camera)
    camera ->GetFeatureByName("AcquisitionStop", pFeature);
    pFeature ->RunCommand();
    
    // Stop the capture engine (API)
    // Flush the frame queue
    // Revoke all frames from the API
    camera ->EndCapture();
    camera ->FlushQueue();
    camera ->RevokeAllFrames();
    for(FramePtrVector::iterator iter=frames.begin(); frames.end()!=iter; ++iter)
    {
        // Unregister the frame observer/callback
        (*iter)->UnregisterObserver();
    }
    camera ->Close();
    sys.Shutdown(); // Always pair sys.Startup and sys.Shutdown
}
}} // namespace AVT::VmbAPI
