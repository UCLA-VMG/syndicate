#include <WinSock2.h>

#include "SpinnakerCamera.h"
#include <iostream>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <unordered_map>

#include "sensorStack.h"


using boost::property_tree::ptree;

int main(int argc, char *argv[]) {
    // Extract XML pTree
    ptree tree;
    std::string XML_PATH("C:/Users/Adnan/Documents/Github/syndicate/scripts/sensor_config.xml");
    read_xml(XML_PATH, tree);

    // Initialize Sensor Stack
    std::cout << std::endl;
    std::cout << "Initializing Sensor Stack" << std::endl;
    SensorStack mainStack(tree);
    
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Acquire Data
    std::cout << std::endl;
    std::cout << "Beginning Acquisition Process Now!" << std::endl;
    mainStack.Acquire();
}