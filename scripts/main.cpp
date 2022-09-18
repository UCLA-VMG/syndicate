#include <winsock2.h>
#include <iostream>
#include <Spinnaker.h>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <unordered_map>

#include "sensor.h"
#include "simpleSensor.h"
#include "sensorStack.h"
#include "SpinnakerCamera.h"
#include "VimbaCamera.h"
#include "SerialPort.h"
#include "OpenCVCamera.h"
#include "RFEthernet.h"
#include "MiniDSPMic.h"
#include "MX800.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello, world!\n";

    std::string current_exec_name = argv[0]; // Name of the current exec program
    std::vector<std::string> all_args;

    if (argc > 1) {
        all_args.assign(argv + 1, argv + argc);
    }
    
    //0. Set Root Path
    // std::string rootPath_C("C:/Users/Adnan/Downloads/patient_3/");
    // std::string rootPath_D("D:/patient_3/");
    std::string rootPath_E("E:/patient_6/");
    // if(all_args.size()>0){rootPath_C = rootPath_C + all_args[0] + "/";}
    // if(all_args.size()>0){rootPath_D = rootPath_D + all_args[0] + "/";}
    if(all_args.size()>0){rootPath_E = rootPath_E + all_args[0] + "/";}
    bool h_sync(true);

    //1. Create Configurations
    // std::unordered_map<std::string, std::any> nir_vimba_config = {
    //     {"Camera ID", std::string("DEV_1AB22C012B3D")}, {"Camera Type", std::string("Vimba")},
    //     {"FPS", 30},
    //     {"Height", 1944}, {"Width", 2592},
    //     {"Sensor Name", std::string("NIR_Vimba_Camera")},
    //     {"Root Path", rootPath_E}, {"Pixel Format", std::string("Mono")},
    //     // {"Hardware Sync", h_sync}, {"Primary", false}
    // };
    std::unordered_map<std::string, std::any> nir_config = { // 940 nm
        {"Camera ID", std::string("21290846")}, {"Camera Type", std::string("Grasshopper3")},
        {"FPS", 30},
        {"Height", 2048}, {"Width", 2048}, // 2048 x 2048
        {"Sensor Name", std::string("NIR_Camera")},
        {"Root Path", rootPath_E}, {"Pixel Format", std::string("Mono")},
        {"Vertical Bin", 2}, {"Horizontal Bin", 2}, // 1024 x 1024
        {"Exposure Compensation", 0.08},
        {"Exposure Time", 30000.0},
        {"Gain", 3.29},
        {"Black Level", 5.57}
        // {"Hardware Sync", h_sync}, {"Primary", false}
    };
    std::unordered_map<std::string, std::any> nir_config_mm = { // 850 nm
        {"Camera ID", std::string("21190637")}, {"Camera Type", std::string("Grasshopper3")},
        {"FPS", 30},
        {"Height", 2048}, {"Width", 2048}, // 2048 
        {"Sensor Name", std::string("NIR_Camera_mm")},
        {"Root Path", rootPath_E}, {"Pixel Format", std::string("Mono")},
        {"Vertical Bin", 2}, {"Horizontal Bin", 2}, // 1024 x 1024
        {"Exposure Compensation", 0.09},
        {"Exposure Time", 30000.0},
        {"Gain", 1.8},
        {"Black Level", 5.08}
        // {"Hardware Sync", h_sync}, {"Primary", false}
    };
    std::unordered_map<std::string, std::any> thermal_config = {
        {"Camera ID", 0}, {"Camera Type", std::string("Boson")},
        {"FPS", 30}, 
        {"Height", 512}, {"Width", 640},
        {"Sensor Name", std::string("Thermal_Camera")},
        {"Root Path", rootPath_E}
    };
    // std::unordered_map<std::string, std::any> rgb_config = {
    //     {"Camera ID", 0}, {"Camera Type", std::string("RGB")},
    //     {"FPS", 30}, 
    //     {"Height", 1280}, {"Width", 720},
    //     {"Sensor Name", std::string("RGB")},
    //     {"Root Path", rootPath}
    // };
    std::unordered_map<std::string, std::any> radar_config = {
        {"data_ip", std::string("192.168.33.30")}, {"adc_ip", std::string("192.168.33.180")}, 
        {"Timeout", 1000},
        {"Packet Size", 1456}, {"Max Packer Size ", 4096},
        {"Sensor Name", std::string("FMCW_Radar")},
        {"Root Path", rootPath_E}
    };
    // std::unordered_map<std::string, std::any> mic_config = {
    //     {"FS", 44100}, {"Channels", 8}, {"Frames per Buffer", 512},
    //     {"Sensor Name", std::string("Microphone")},
    //     {"Root Path", rootPath}
    // };
    // std::unordered_map<std::string, std::any> mx800_config = {
    //     {"Exe Path", std::string("C:/Users/111/Desktop/repos/mmhealth_2/sensors/VSCaptureMP-master-copy/VSCaptureMP/VSCaptureMP/bin/Debug/VSCaptureMP.exe")},
    //     {"Sensor Name", std::string("MX800")},
    //     {"Root Path", rootPath}
    // };
    std::unordered_map<std::string, std::any> serial_config = {
        {"Port Name", std::string("\\\\.\\COM14")},
        {"Pulse Time", 1}, {"Total Time", 20},
        {"Sensor Name", std::string("Arduino_Serial")},
        {"Root Path", rootPath_E}
    };


    // std::unordered_map<std::string, std::any> thermal_config = {
    //     {"Camera ID", 0}, {"Camera Type", std::string("Boson")},
    //     {"FPS", 30}, 
    //     {"Height", 512}, {"Width", 640},
    //     {"Sensor Name", std::string("Thermal_Camera")},
    //     {"Root Path", rootPath_E}
    // };
    
    //2. Add Configurations and Factory Generator Functions into std::vectors
    std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)> sensor_list;
    // sensor_list.emplace_back(makeSensor<VimbaCamera>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    sensor_list.emplace_back(makeSensor<OpenCVCamera>);
    sensor_list.emplace_back(makeSensor<RFEthernet>);
    sensor_list.emplace_back(makeSensor<SerialPort>);

    std::vector<std::unordered_map<std::string, std::any>> configs{nir_config, nir_config_mm, thermal_config, radar_config, serial_config};
    
    //3. Initialize Sensor Stack
    SensorStack mainStack(sensor_list, configs);
    
    //4. Acquire Data
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    //4.1 Asynchronously Acquire Data
    std::cout << "\n\n\nAsyn Capture \n";
    mainStack.Acquire(21600); // 10,800 [3 hrs] & 14,400 [4 hrs] & 21,600 [6 hrs]

    // 4.2 Barrier Sync Acquire Data
    // std::cout << "\n\n\n Barrier Sync Capture\n";
    // mainStack.AcquireBarrier(5);

    //4.3 Barrier Acquire on One thread, and save asynchronously on another thread.
    // std::cout << "\n\n\n Barrier Sync Capture and Asynch Save\n";
    // mainStack.ConcurrentAcquireSave(30);


    //Test base class calling derived class func
    // SimpleSensor a(sample_config);
    // Sensor* b = nullptr;
    // b = &a;
    // b->AcquireSave(10);
}