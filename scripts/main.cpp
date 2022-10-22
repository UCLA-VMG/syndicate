#include <WinSock2.h>

#include <iostream>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <unordered_map>

#include "sensorStack.h"
#include "SerialPort.h"

using boost::property_tree::ptree;

int main(int argc, char *argv[]) {
    // Extract XML pTree
    ptree tree;
    std::string XML_PATH("C:/Users/Adnan/Documents/Github/syndicate/scripts/sensor_config.xml");
    read_xml(XML_PATH, tree);
    ptree & ptree_global = tree.get_child("sensor_stack").get_child("global");

    std::string rootPath = ptree_global.get<std::string>("save_path");
    // Add a '/' to the end if it is not present
    if (rootPath[rootPath.length()-1] != '/' || rootPath[rootPath.length()-1] != '\\')
        rootPath += '/';
    // Create the main folder
    std::filesystem::create_directory(rootPath);
    // Iterate over the different trials
    for (int i = 0; i < ptree_global.get<int>("num_trials"); i++) {
        std::string trial_id = std::to_string(i);
        std::cout << "Waiting Before Trial " << trial_id << "!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(ptree_global.get<int>("wait_time_" + trial_id)));
        std::cout << "Waiting Done!" << std::endl;
        ptree_global.put<std::string>("save_path", rootPath + "trial_" + trial_id + '/');
        
        std::cout << "Saving to " <<  tree.get_child("sensor_stack").get_child("global").get<std::string>("save_path") << std::endl;
        
        // Create the trial folders
        std::filesystem::create_directory(ptree_global.get<std::string>("save_path"));

        // // Initialize Sensor Stack
        std::cout << std::endl;
        std::cout << "Initializing Sensor Stack" << std::endl;
        SensorStack mainStack(tree);
        
        // Acquire Data
        std::cout << std::endl;
        std::cout << "Beginning Acquisition Process Now!" << std::endl;
        mainStack.Acquire(); // 10,800 [3 hrs] & 14,400 [4 hrs] & 21,600 [6 hrs]
        
        // Reset the FPGA
        ptree::value_type global_config(std::pair("global", ptree_global));
        ptree::value_type sensor_config(std::pair("sensor", tree.get_child("sensor_stack").get_child("reset_sensor")));
        SerialPort reset_arduino(sensor_config, global_config);
        std::cout << "Beginning Reset Process!" << std::endl;
        reset_arduino.resetFPGA();
        std::cout << "Ended Reset Process!" << std::endl;
    }
}