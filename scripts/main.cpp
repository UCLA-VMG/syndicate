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
#include "RealSenseCamera.h"

// int main(int argc, char *argv[]) {
//     std::cout << "Hello, world!\n";

//     std::string current_exec_name = argv[0]; // Name of the current exec program
//     std::vector<std::string> all_args;

int main(int argc, char *argv[]) {
    std::cout << "Hello, world!\n";
    
    //0. Set Root Path
    std::string rootPath("D:/BP_RF_CAM");
    if (argc > 1){
        rootPath = rootPath + std::string("/") + std::string(argv[1]) + std::string("/");
    }
    else {
        rootPath = rootPath + std::string("/");
    }

    std::cout << rootPath << std::endl << std::endl; 
    bool h_sync(false);

    //1. Create Configurations
    std::unordered_map<std::string, std::any> nir_config = {
        {"Camera ID", std::string("21290846")}, {"Camera Type", std::string("Grasshopper3")},
        {"FPS", 30},
        {"Height", 2048}, {"Width", 2048},
        {"Sensor Name", std::string("NIR_Camera")},
        {"Root Path", rootPath}, {"Pixel Format", std::string("Mono")},
        {"Hardware Sync", h_sync}, {"Primary", false}
    };
    std::unordered_map<std::string, std::any> nir_vimba_config = {
        {"Camera ID", std::string("DEV_1AB22C012B3D")}, {"Camera Type", std::string("Vimba")},
        {"FPS", 30},
        {"Height", 1944}, {"Width", 2592},
        {"Sensor Name", std::string("NIR_Vimba_Camera")},
        {"Root Path", rootPath}, {"Pixel Format", std::string("Mono")},
        {"Hardware Sync", h_sync}, {"Primary", false}
    };
    std::unordered_map<std::string, std::any> polarized_config = {
        {"Camera ID", std::string("19224369")}, {"Camera Type", std::string("BackflyS")},
        {"FPS", 30},
        {"Height", 2048}, {"Width", 2448},
        {"Sensor Name", std::string("Polarized_Camera")},
        {"Root Path", rootPath}, {"Pixel Format", std::string("Mono")},
        {"Hardware Sync", h_sync}, {"Primary", false}
    };
    std::unordered_map<std::string, std::any> thermal_config = {
        {"Camera ID", 0}, {"Camera Type", std::string("Boson")},
        {"FPS", 30}, {"Bit Depth", 16},
        {"Height", 512}, {"Width", 640},
        {"Sensor Name", std::string("Thermal_Camera")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> rgb_config = {
        {"Camera ID", 0}, {"Camera Type", std::string("RGB")},
        {"FPS", 30}, 
        {"Height", 3840}, {"Width", 2060},
        {"Sensor Name", std::string("RGB")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> radar_config = {
        {"data_ip", std::string("192.168.33.30")}, {"adc_ip", std::string("192.168.33.180")}, 
        {"Timeout", 1000},
        {"Packet Size", 1456}, {"Max Packer Size ", 4096},
        {"Sensor Name", std::string("FMCW_Radar")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> mic_config = {
        {"FS", 44100}, {"Channels", 2}, {"Frames per Buffer", 512},
        {"Sensor Name", std::string("Microphone")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> mx800_config = {
        {"Exe Path", std::string("C:/Users/111/Desktop/repos/mmhealth_2/sensors/VSCaptureMP-master-copy/VSCaptureMP/VSCaptureMP/bin/Debug/VSCaptureMP.exe")},
        {"Sensor Name", std::string("MX800")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> real_sense_sr300_config = {
        {"Camera ID", 0}, {"Camera Type", std::string("SR300")},
        {"FPS", 30}, 
        {"Height", 480}, {"Width", 640},
        {"Sensor Name", std::string("Coded_Light_Depth_Camera")},
    };
    std::unordered_map<std::string, std::any> serial_config = {
        {"Port Name", std::string("\\\\.\\COM14")},
        {"Pulse Time", 1}, {"Total Time", 20},
        {"Sensor Name", std::string("Arduino_Serial")},
        {"Root Path", rootPath}
    };
    
    //2. Add Configurations and Factory Generator Functions into std::vectors
    std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)> sensor_list;
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    // sensor_list.emplace_back(makeSensor<OpenCVCamera>);
    // sensor_list.emplace_back(makeSensor<VimbaCamera>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    //sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
   //sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    // sensor_list.emplace_back(makeSensor<MiniDSPMic>);
    // sensor_list.emplace_back(makeSensor<RealSenseCamera>);
    sensor_list.emplace_back(makeSensor<RFEthernet>);
    sensor_list.emplace_back(makeSensor<MX800>);

    std::vector<std::unordered_map<std::string, std::any>> configs{polarized_config, radar_config, mx800_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{rgb_config, nir_vimba_config, nir_config, mx800_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{polarized_config};
    //std::vector<std::unordered_map<std::string, std::any>> configs{nir_config, polarized_config, mic_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{mic_config , real_sense_sr300_config, radar_config, mx800_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{mic_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{rgb_config, nir_config, polarized_config, real_sense_sr300_config}; //thermal_config
    // std::vector<std::unordered_map<std::string, std::any>> configs{rgb_config, nir_config, polarized_config, mic_config, real_sense_sr300_config, radar_config, mx800_config}; //thermal_config
    
    // # Adnan, your stuff
    // sensor_list.emplace_back(makeSensor<VimbaCamera>);
    // sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    // sensor_list.emplace_back(makeSensor<OpenCVCamera>);
    // sensor_list.emplace_back(makeSensor<RFEthernet>);
    // sensor_list.emplace_back(makeSensor<SerialPort>);
    // std::vector<std::unordered_map<std::string, std::any>> configs{nir_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{nir_vimba_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{thermal_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{radar_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{serial_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{nir_vimba_config, nir_config, thermal_config, serial_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{nir_vimba_config, nir_config, thermal_config, radar_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{nir_vimba_config, nir_config, rgb_config};

    
    //3. Initialize Sensor Stack
    std::cout << "\n\n\n Barrier Sync Capture\n";

    SensorStack mainStack(sensor_list, configs);
    
    //4. Acquire Data
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    //4.1 Asynchronously Acquire Data
    std::cout << "\n\n\nAsyn Capture \n";
    mainStack.Acquire(60);


    // 4.2 Barrier Sync Acquire Data
    std::cout << "\n\n\n Barrier Sync Capture\n";
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