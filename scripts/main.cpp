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
#include "OpenCVCamera.h"
#include "RFEthernet.h"
#include "MiniDSPMic.h"
#include "MX800.h"

boost::mutex io_mutex;

void thread_fun(boost::barrier& cur_barier, boost::atomic<int>& current)
{
    ++current;
    cur_barier.wait();
    boost::lock_guard<boost::mutex> locker(io_mutex);
    std::cout << current << std::endl;
}

int main(int, char**) {
    std::cout << "Hello, world!\n";
    
    //0. Set Root Path
    std::string rootPath("D:/syndicate_tests/");
    bool h_sync(true);

    //1. Create Configurations
    std::unordered_map<std::string, std::any> sample_config = {
        {"FPS", 60},
        {"Sensor Name", std::string("Simp1")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> sample_config2 = {
        {"FPS", 60},
        {"Sensor Name", std::string("Simp2")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> nir_config = {
        {"Camera ID", std::string("21190637")}, {"Camera Type", std::string("Grasshopper3")},
        {"FPS", 30},
        {"Height", 2048}, {"Width", 2048},
        {"Sensor Name", std::string("NIR_Camera")},
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
    std::unordered_map<std::string, std::any> rgb_config = {
        {"Camera ID", std::string("21502645")}, {"Camera Type", std::string("BackflyS")},
        {"FPS", 30},{"FPS Correction", 0}, 
        {"Height", 1100}, {"Width", 1600},
        {"Sensor Name", std::string("RGB_Camera")},
        {"Root Path", rootPath}, {"Pixel Format", std::string("RGB")},
        {"Hardware Sync", h_sync}, {"Primary", true}
    };
    std::unordered_map<std::string, std::any> thermal_config = {
        {"Camera ID", 0}, {"Camera Type", "Boson"},
        {"FPS", 30}, 
        {"Height", 512}, {"Width", 640},
        {"Sensor Name", std::string("Thermal_Camera")},
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
        {"FS", 44100}, {"Channels", 8}, {"Frames per Buffer", 512},
        {"Sensor Name", std::string("Microphone")},
        {"Root Path", rootPath}
    };
    std::unordered_map<std::string, std::any> mx800_config = {
        {"Exe Path", std::string("C:/Users/111/Desktop/repos/mmhealth_2/sensors/VSCaptureMP-master-copy/VSCaptureMP/VSCaptureMP/bin/Debug/VSCaptureMP.exe")},
        {"Sensor Name", std::string("MX800")},
        {"Root Path", rootPath}
    };
    
    //2. Add Configurations and Factory Generator Functions into std::vectors
    std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)> sensor_list;
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    // sensor_list.emplace_back(makeSensor<OpenCVCamera>);
    // sensor_list.emplace_back(makeSensor<RFEthernet>);
    // sensor_list.emplace_back(makeSensor<MiniDSPMic>);
    // sensor_list.emplace_back(makeSensor<MX800>);
    std::vector<std::unordered_map<std::string, std::any>> configs{rgb_config, nir_config, polarized_config};//, mx800_config};
    // std::vector<std::unordered_map<std::string, std::any>> configs{rgb_config, nir_config, polarized_config, thermal_config, radar_config, mic_config, mx800_config};
    
    
    //3. Initialize Sensor Stack
    SensorStack mainStack(sensor_list, configs);
    
    //4. Acquire Data

    //4.1 Asynchronously Acquire Data
    std::cout << "\n\n\nAsyn Capture \n";
    mainStack.Acquire(10);

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