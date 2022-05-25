// Every Vimba function returns an error code that
// should always be checked for VmbErrorSuccess
VmbErrorType err; 


VimbaSystem &sys; // A reference to the VimbaSystem singleton
CameraPtrVector cameras; // A list of known cameras
// A list of frames for streaming. We chose
// to queue 3 frames.
FramePtrVector frames( 3 );

// Our implementation
// of a frame observer
IFrameObserverPtr pObserver( new MyFrameObserver() ); 

FeaturePtr pFeature; // Any camera feature
VmbInt64_t nPLS; // The payload size of one frame

sys = VimbaSystem::GetInstance();
err = sys.Startup():
err = sys.GetCameras( cameras );
err = cameras[0]->Open( VmbAccessModeFull );

err = cameras[0]->GetFeatureByName( "PayloadSize", pFeature );
err = pFeature ->GetValue( nPLS )

for ( FramePtrVector::iterator iter = frames.begin(); frames.end() != iter; ++iter )
{
    ( *iter ).reset( new Frame( nPLS ) );
    err = ( *iter )->RegisterObserver( pObserver ) );
    err = cameras[0]->AnnounceFrame( *iter );
}

err = cameras[0]->StartCapture();

for ( FramePtrVector::iterator iter = frames.begin(); frames.end() != iter; ++iter )
{
    err = cameras[0]->QueueFrame( *iter ); 
}

err = GetFeatureByName( "AcquisitionStart", pFeature );
err = pFeature ->RunCommand();

// Program runtime ...

// When finished , tear down the acquisition chain , close the camera and Vimba
err = GetFeatureByName( "AcquisitionStop", pFeature );
err = pFeature ->RunCommand();
err = cameras[0]->EndCapture();
err = cameras[0]->FlushQueue();
err = cameras[0]->RevokeAllFrames();
err = cameras[0]->Close();
err = sys.Shutdown();