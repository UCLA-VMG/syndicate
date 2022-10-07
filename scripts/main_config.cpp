#include <WinSock2.h>

#include "SpinnakerCamera.h"
#include <iostream>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <unordered_map>

#include "sensor.h"
// #include "simpleSensor.h"
#include "sensorStack.h"
// #include "VimbaCamera.h"
// #include "SerialPort.h"
// #include "OpenCVCamera.h"
#include "RFEthernet.h"
// #include "MiniDSPMic.h"
#include "MX800.h"
// #include "RealSenseCamera.h"

using boost::property_tree::ptree;

int main(int argc, char *argv[]) {

    //1. Extract XML pTree
    ptree tree;
    std::string XML_PATH("C:/Users/111/Desktop/repos/syndicate/scripts/test_mx800.xml");
    read_xml(XML_PATH, tree);
    
    //2. Add Configurations and Factory Generator Functions into std::vectors
    // std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)> sensor_list;
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    // sensor_list.emplace_back(makeSensor<OpenCVCamera>);
    // sensor_list.emplace_back(makeSensor<VimbaCamera>);
    // sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    //sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
   //sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    // sensor_list.emplace_back(makeSensor<MiniDSPMic>);
    // sensor_list.emplace_back(makeSensor<RealSenseCamera>);
    // sensor_list.emplace_back(makeSensor<RFEthernet>);
    
    //3. Initialize Sensor Stack
    std::cout << "\n\n\n Barrier Sync Capture\n";

    SensorStack mainStack(tree);
    
    // 4. Acquire Data
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // 4.1 Asynchronously Acquire Data
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