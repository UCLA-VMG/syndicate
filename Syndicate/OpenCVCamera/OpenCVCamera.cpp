#include "OpenCVCamera.h"

using namespace cv;
using namespace std;

using namespace Syndicate;

OpenCVCamera::OpenCVCamera(std::unordered_map<std::string, std::any>& sample_config)
    : Syndicate::Camera(sample_config), cameraID(std::any_cast<int>(sample_config["Camera ID"]))
{
    std::cout << "opencv Camera Cstr\n";
    //--- INITIALIZE MAT
    Mat frame;
    std::cout << "opencv Camera Cstr2\n";
    //--- OPEN CAP
    // cameraID = 0;
    // std::cout << "opencv Camera Cstr3\n";

    cap = openCap(cameraID);
    std::cout << "opencv Camera Cstr4\n";

}

OpenCVCamera::~OpenCVCamera()
{
    // Deinitialize camera
    // the camera will be deinitialized automatically in VideoCapture destructor
}

void OpenCVCamera::AcquireSave(double seconds)
{
    int num_frames(int(seconds)*int(fps));
    char filename[100];
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    auto start = chrono::steady_clock::now();
    try {
        for (int i = 0; i < num_frames; i++)
        {
            // wait for a new frame from camera and store it into 'frame'
            cap.read(frame);
            cout << "currentDateTime()=" << currentDateTime() << endl;

            if(!frame.empty() ) {
                sprintf_s(filename, "C:/Users/Adnan/Downloads/test/Images/Frame_%d.png", i); // select your folder - filename is "Frame_n"
                imwrite(filename, frame);
                cout << "Frame_" << i << endl;
            }
        }
    }
    catch (Exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    auto end = chrono::steady_clock::now();
    cout << "Time Taken for " << sensorName << (end-start).count() << "\n";
}

void OpenCVCamera::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier)
{
    int num_frames(int(seconds)*int(fps));
    char filename[100];
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    auto start = chrono::steady_clock::now();
    try {
        for (int i = 0; i < num_frames; i++)
        {
            frameBarrier.wait();
            // wait for a new frame from camera and store it into 'frame'
            cap.read(frame);
            cout << "currentDateTime()=" << currentDateTime() << endl;

            if(!frame.empty() ) {
                sprintf_s(filename, "C:/Users/Adnan/Downloads/test/Images/Frame_%d.png", i); // select your folder - filename is "Frame_n"
                imwrite(filename, frame);
                cout << "Frame_" << i << endl;
            }
        }
    }
    catch (Exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    auto end = chrono::steady_clock::now();
    cout << "Time Taken for " << sensorName << (end-start).count() << "\n";
}

bool setResolution(VideoCapture cap, double width, double height) {
    bool result = true;
    try {
        cap.set(CAP_PROP_FRAME_WIDTH, width);
        cout << "Width after using video.set(CAP_PROP_FRAME_WIDTH) : " << cap.get(CAP_PROP_FRAME_WIDTH) << endl;
        cap.set(CAP_PROP_FRAME_HEIGHT, height);
        cout << "Height after using video.set(CAP_PROP_FRAME_HEIGHT) : " << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring resolution: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool setFps(VideoCapture cap, double fps) {
    bool result = true;
    try {
        cap.set(CAP_PROP_FPS, fps);
        cout << "Frames per second using video.get(CAP_PROP_FPS) : " << fps << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring fps: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool configure(VideoCapture cap, int cameraID, double fps = 30.0, double height = 640, double width = 640) {
    bool result = true;
    try {
        // Print the device serial number
        cout << "Configuring device " << cameraID << endl;

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
        cout << "Error configuring camera: " << e.what() << endl;
        result = false;
    }
    return result;
}

// This function acquires and saves 10 images from a device.
bool OpenCVCamera::AcquireImages(VideoCapture cap, int num_frames) {
    bool result = true;
    char filename[100];
    cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
    try {
        for (int i = 0; i < num_frames; i++)
        {
            // wait for a new frame from camera and store it into 'frame'
            cap.read(frame);
            cout << "currentDateTime()=" << currentDateTime() << endl;

            if(!frame.empty() ) {
                sprintf_s(filename, "C:/Users/Adnan/Downloads/test/Images/Frame_%d.png", i); // select your folder - filename is "Frame_n"
                imwrite(filename, frame);
                cout << "Frame_" << i << endl;
            }
        }
    }
    catch (Exception& e) {
        cout << "Error: " << e.what() << endl;
        return false;
    }
    return result;
}

bool getResolution(VideoCapture cap) {
    bool result = true;
    try {
        double width = cap.get(CAP_PROP_FRAME_WIDTH);
        cout << "Width using video.get(CAP_PROP_FRAME_WIDTH) : " << width << endl;
        double height = cap.get(CAP_PROP_FRAME_HEIGHT);
        cout << "Height using video.get(CAP_PROP_FRAME_HEIGHT) : " << height << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring resolution: " << e.what() << endl;
        result = false;
    }
    return result;
}

bool getFps(VideoCapture cap) {
    bool result = true;
    try {
        double fps = cap.get(CAP_PROP_FPS);
        cout << "Frames per second using video.get(CAP_PROP_FPS) : " << fps << endl;
    }
    catch (Exception& e) {
        cout << "Error configuring fps: " << e.what() << endl;
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
        cerr << "ERROR! Unable to open camera\n";
    }
    return cap;
}

const string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}