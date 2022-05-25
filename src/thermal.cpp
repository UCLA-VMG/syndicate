#include "thermal.h"

using namespace cv;
using namespace std;

int ct = 0; 
char tipka;
char filename[100]; // For filename
int  c = 1; // For filename
const int num_frames = 300;

void get_fps(VideoCapture cap) {
    double fps = cap.get(CAP_PROP_FPS);
    cout << "Frames per second using video.get(CAP_PROP_FPS) : " << fps << endl;
}

void get_resolution(VideoCapture cap) {
    double width = cap.get(CAP_PROP_FRAME_WIDTH);
    cout << "Width using video.get(CAP_PROP_FRAME_WIDTH) : " << width << endl;
    double height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cout << "Height using video.get(CAP_PROP_FRAME_HEIGHT) : " << height << endl;
}

void set_fps(VideoCapture cap, double fps) {
    cap.set(CAP_PROP_FPS, fps);
    cout << "Frames per second using video.get(CAP_PROP_FPS) : " << fps << endl;
}

void set_resolution(VideoCapture cap, double width, double height) {
    cap.set(CAP_PROP_FRAME_WIDTH, width);
    cout << "Width after using video.set(CAP_PROP_FRAME_WIDTH) : " << cap.get(CAP_PROP_FRAME_WIDTH) << endl;
    cap.set(CAP_PROP_FRAME_HEIGHT, height);
    cout << "Height after using video.set(CAP_PROP_FRAME_HEIGHT) : " << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
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

int main(int, char**)
{
    Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;

    // open the default camera using default API
    // cap.open(0);
    // // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera   
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
                                  // open selected camera using selected API
    cap.open(deviceID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    // check fps
    get_fps(cap);
    // check resolution
    get_resolution(cap);

    // set fps
    set_fps(cap, 30.0);
    // set resolution
    // set_resolution(cap, 640, 512);

    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press a to terminate" << endl;
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
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}