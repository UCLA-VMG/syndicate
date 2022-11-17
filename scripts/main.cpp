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
    std::string XML_PATH;
    if(argc == 2){XML_PATH = argv[1];}
    else{XML_PATH ="C:/Users/111/Desktop/repos/syndicate/scripts/sensor_config_spo2.xml";}
    
    read_xml(XML_PATH, tree);

    //3. Initialize Sensor Stack
    std::cout << "\n\n\n Starting Barrier Sync Capture\n";

    SensorStack mainStack(tree);
    
    // 4. Acquire Data
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 4.1 Asynchronously Acquire Data
    std::cout << "\n\n\nAsyn Capture \n";
    mainStack.Acquire();
}