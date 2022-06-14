#include "OpenCVCamera.h"

using namespace cv;

using namespace Syndicate;

OpenCVCamera::OpenCVCamera(std::unordered_map<std::string, std::any>& sample_config)
    : Syndicate::Camera(sample_config), cameraID(std::any_cast<int>(sample_config["Camera ID"]))
{
    std::cout << "opencv Camera Cstr\n";
    //--- OPEN CAP
    cap = openCap(cameraID);
}

OpenCVCamera::~OpenCVCamera()
{
    // Deinitialize camera
    // the camera will be deinitialized automatically in VideoCapture destructor
}

void OpenCVCamera::AcquireSave(double seconds, boost::barrier& startBarrier)
{
    int num_frames(int(seconds)*int(fps));
    std::cout << std::endl << std::endl << "*** IMAGE ACQUISITION ***" << std::endl << std::endl;
    auto start = std::chrono::steady_clock::now();
    startBarrier.wait();
    try {
        for (int i = 0; i < num_frames; i++)
        {
            cv::Mat frame;
            // Wait for a new frame from camera and store it into 'frame'
            cap.read(frame);
            RecordTimeStamp();
            if(!frame.empty() ) {
                runningBuffer.push(frame);
            }
            
        }
    }
    catch (Exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time Taken for " << sensorName  << " " << float((end-start).count())/1'000'000'000 << "\n";
    SaveTimeStamps();

    start = std::chrono::steady_clock::now();
    cv::Mat save_frame;
    for (int i = 0; i < num_frames; i++) {
        save_frame = std::any_cast<cv::Mat>(runningBuffer.front());
        // Create a unique filename
        std::ostringstream filename;
        filename << rootPath << sensorName << "_" << i << ".png";
        imwrite(filename.str().c_str(), save_frame);
        runningBuffer.pop();
        // sprintf_s(filename, filename.str().c_str()); // select your folder - filename is "Frame_n"
        // std::cout << sensorName <<"_Frame_" << i << std::endl;
    }
    end = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for Saving " << sensorName << " " << float((end-start).count())/1'000'000'000 << "\n";
    this->setHealthCode(HealthCode::ONLINE);
}

void OpenCVCamera::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier)
{
    int num_frames(int(seconds)*int(fps));
    char filename[100];
    std::cout << std::endl << std::endl << "*** IMAGE ACQUISITION ***" << std::endl << std::endl;
    auto start = std::chrono::steady_clock::now();
    try {
        for (int i = 0; i < num_frames; i++)
        {
            frameBarrier.wait();
            // wait for a new frame from camera and store it into 'frame'
            cv::Mat frame;
            cap.read(frame);
            std::cout << "currentDateTime()=" << currentDateTime() << std::endl;

            if(!frame.empty() ) {
                sprintf_s(filename, "%s/%s_%d.png", rootPath, sensorName, i); // select your folder - filename is "Frame_n"
                imwrite(filename, frame);
                std::cout << "Frame_" << i << std::endl;
            }
        }
    }
    catch (Exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time Taken for " << sensorName << (end-start).count() << "\n";
}

bool setResolution(VideoCapture cap, double width, double height) {
    bool result = true;
    try {
        cap.set(CAP_PROP_FRAME_WIDTH, width);
        std::cout << "Width after using video.set(CAP_PROP_FRAME_WIDTH) : " << cap.get(CAP_PROP_FRAME_WIDTH) << std::endl;
        cap.set(CAP_PROP_FRAME_HEIGHT, height);
        std::cout << "Height after using video.set(CAP_PROP_FRAME_HEIGHT) : " << cap.get(CAP_PROP_FRAME_HEIGHT) << std::endl;
    }
    catch (Exception& e) {
        std::cout << "Error configuring resolution: " << e.what() << std::endl;
        result = false;
    }
    return result;
}

bool setFps(VideoCapture cap, double fps) {
    bool result = true;
    try {
        cap.set(CAP_PROP_FPS, fps);
        std::cout << "Frames per second using video.get(CAP_PROP_FPS) : " << fps << std::endl;
    }
    catch (Exception& e) {
        std::cout << "Error configuring fps: " << e.what() << std::endl;
        result = false;
    }
    return result;
}

bool configure(VideoCapture cap, int cameraID, double fps = 30.0, double height = 640, double width = 640) {
    bool result = true;
    try {
        // Print the device serial number
        std::cout << "Configuring device " << cameraID << std::endl;

        // set fps
        if (!setFps(cap, fps)) {
            return false;
        }
        // set resolution
        if (!setResolution(cap, height, width)) {
            return false;
        }
    }
    catch (Exception& e) {
        std::cout << "Error configuring camera: " << e.what() << std::endl;
        result = false;
    }
    return result;
}

// This function acquires and saves 10 images from a device.
bool OpenCVCamera::AcquireImages(VideoCapture cap, int num_frames) {
    bool result = true;
    char filename[100];
    std::cout << std::endl << std::endl << "*** IMAGE ACQUISITION ***" << std::endl << std::endl;
    try {
        for (int i = 0; i < num_frames; i++)
        {
            // wait for a new frame from camera and store it into 'frame'
            cv::Mat frame;
            cap.read(frame);
            std::cout << "currentDateTime()=" << currentDateTime() << std::endl;

            if(!frame.empty() ) {
                sprintf_s(filename, "C:/Users/Adnan/Downloads/test/Images/Frame_%d.png", i); // select your folder - filename is "Frame_n"
                imwrite(filename, frame);
                std::cout << "Frame_" << i << std::endl;
            }
        }
    }
    catch (Exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
    return result;
}

bool getResolution(VideoCapture cap) {
    bool result = true;
    try {
        double width = cap.get(CAP_PROP_FRAME_WIDTH);
        std::cout << "Width using video.get(CAP_PROP_FRAME_WIDTH) : " << width << std::endl;
        double height = cap.get(CAP_PROP_FRAME_HEIGHT);
        std::cout << "Height using video.get(CAP_PROP_FRAME_HEIGHT) : " << height << std::endl;
    }
    catch (Exception& e) {
        std::cout << "Error configuring resolution: " << e.what() << std::endl;
        result = false;
    }
    return result;
}

bool getFps(VideoCapture cap) {
    bool result = true;
    try {
        double fps = cap.get(CAP_PROP_FPS);
        std::cout << "Frames per second using video.get(CAP_PROP_FPS) : " << fps << std::endl;
    }
    catch (Exception& e) {
        std::cout << "Error configuring fps: " << e.what() << std::endl;
        result = false;
    }
    return result;
}

VideoCapture OpenCVCamera::openCap(int cameraID) {
    //--- INITIALIZE CAP
    VideoCapture cap;
    // open the default camera using default API
    // cap.open(0);
    // // OR advance usage: select any API backend 
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
                                  // open selected camera using selected API
    cap.open(cameraID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
    }
    return cap;
}

void OpenCVCamera::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    std::cout << "I am not defined yet.\n\n";
}

void OpenCVCamera::ConcurrentSave()
{
    std::cout << "I am not defined yet.\n\n";
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}