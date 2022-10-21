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

const int NUM_TRIALS = 3;

int main(int argc, char *argv[]) {
    // Extract XML pTree
    ptree tree;
    std::string XML_PATH("C:/Users/Adnan/Documents/Github/syndicate/scripts/sensor_config.xml");
    read_xml(XML_PATH, tree);
    ptree & ptree_global = tree.get_child("sensor_stack").get_child("global");

    std::cout << "Waiting Before Trial 0!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(ptree_global.get<int>("wait_time_0"))); // wait 5400 seconds for main script to execute (give patient chance to sleep)
    std::cout << "Waiting Done!" << std::endl;

    for (int i = 0; i < NUM_TRIALS; i++) {
        if (i == 0) {
            ptree_global.put<std::string>("save_path", ptree_global.get<std::string>("save_path_0"));
        }
        if (i == 1) {
            std::cout << "Waiting Before Trial 1!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(ptree_global.get<int>("wait_time_1"))); // wait 5400 seconds for main script to execute (give patient chance to sleep)
            std::cout << "Waiting Done!" << std::endl;
            ptree_global.put<std::string>("save_path", ptree_global.get<std::string>("save_path_1"));
        }
        if (i == 2) {
            std::cout << "Waiting Before Trial 2!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(ptree_global.get<int>("wait_time_2"))); // wait 5400 seconds for main script to execute (give patient chance to sleep)
            std::cout << "Waiting Done!" << std::endl;
            ptree_global.put<std::string>("save_path", ptree_global.get<std::string>("save_path_2"));
        }
        
        std::cout << "Saving to " <<  tree.get_child("sensor_stack").get_child("global").get<std::string>("save_path") << std::endl;

        // Initialize Sensor Stack
        std::cout << std::endl;
        std::cout << "Initializing Sensor Stack" << std::endl;
        SensorStack mainStack(tree);
        
        // Acquire Data
        std::cout << std::endl;
        std::cout << "Beginning Acquisition Process Now!" << std::endl;
        mainStack.Acquire(); // 10,800 [3 hrs] & 14,400 [4 hrs] & 21,600 [6 hrs]
    }
}