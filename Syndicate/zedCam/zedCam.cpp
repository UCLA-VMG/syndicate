#include "zedCam.h"

using namespace Syndicate;
using namespace sl;

void print(std::string msg_prefix, sl::ERROR_CODE err_code = ERROR_CODE::SUCCESS, std::string msg_suffix = "") {
    std::cout <<"[Sample]";
    if (err_code != sl::ERROR_CODE::SUCCESS)
        std::cout << "[Error] ";
    else
        std::cout<<" ";
    std::cout << msg_prefix << " ";
    if (err_code != sl::ERROR_CODE::SUCCESS) {
        std::cout << " | " << toString(err_code) << " : ";
        std::cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        std::cout << " " << msg_suffix;
    std::cout << std::endl;
}

zedCam::zedCam(ptree::value_type& sensor_settings, ptree::value_type& global_settings)
    : Syndicate::Camera(sensor_settings, global_settings), cameraID(0)
{
    if (sensor_settings.second.get<bool>("hardware_sync") == 0) {
        hardware_sync = false;
    }
    else {
        hardware_sync = true;
    }
    std::cout << std::endl << std::endl << "Configuring " << sensorName << "\n";
    //--- OPEN CAP
    // Set configuration parameters for the ZED
    sl::InitParameters init_parameters;
    init_parameters.camera_resolution = sl::RESOLUTION::HD1080;
    init_parameters.depth_mode = sl::DEPTH_MODE::NONE;
    // Open the camera
    auto returned_state  = zed.open(init_parameters);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        print("Camera Open", returned_state, "Exit program.");
    }
    // Enable recording with the filename specified in argument
    std::string path_output = rootPath + sensorName + "_output.svo";
    String path_output_sl(path_output.c_str());
    returned_state = zed.enableRecording(RecordingParameters(path_output_sl, sl::SVO_COMPRESSION_MODE::H264_LOSSLESS));
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        print("Recording ZED : ", returned_state, "Exit program.");
        zed.close();
    }
    std::cout << "Zed2 Camera Setup.\n";
}

zedCam::~zedCam()
{
    // Deinitialize camera
    // the camera will be deinitialized automatically in VideoCapture destructor
}

void zedCam::AcquireSave(double seconds, boost::barrier& startBarrier)
{
    int num_frames(int(seconds)*int(fps));
    // int num_frames(int(seconds)*int(fps));
    std::cout << std::endl << std::endl << "*** IMAGE ACQUISITION ***" << std::endl << std::endl;
    startBarrier.wait();
    auto start = std::chrono::steady_clock::now();
    

    int frames_recorded = 0;
    sl::RecordingStatus rec_status;
    while (frames_recorded < num_frames) {
        if (zed.grab() == sl::ERROR_CODE::SUCCESS) {
            // Each new frame is added to the SVO file
            rec_status = zed.getRecordingStatus();
            if (rec_status.status)
                RecordTimeStamp();
                frames_recorded++;
            print("Frame count: " + std::to_string(frames_recorded));
        }
    }
    // end acquisition 
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time Taken for " << sensorName  << " " << float((end-start).count())/1'000'000'000 << "\n";
    SaveTimeStamps();
    this->setHealthCode(HealthCode::ONLINE);
    // Stop recording
    zed.disableRecording();
    zed.close();
}
