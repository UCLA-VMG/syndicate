#include "sensorStack.h"

SensorStack::SensorStack(boost::property_tree::ptree tree)
{
    using boost::property_tree::ptree;
 
    std::vector<std::unique_ptr<Sensor>(*)(ptree::value_type&, ptree::value_type&)> sensor_list;
    std::vector<ptree::value_type> local_configs;
    ptree::value_type global_config;
    // ptree::value_type setting_config;
    std::string ATTR_SET("sensor");
    std::string GLOBAL("global");
 
    BOOST_FOREACH(ptree::value_type & f, tree.get_child("sensor_stack")){
        std::string at = f.first;
        std::cout << at << " \n";
        if(at == ATTR_SET)
        {
            /// @Adnan This needs to be modified for all sensors.
            // Check if sensor is in list of defined sensors
            if(f.second.get<std::string>("type") == "MX800")
                sensor_list.emplace_back(makeSensor<MX800>);
            else if(f.second.get<std::string>("type") == "SpinnakerCamera")
                sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
            else if(f.second.get<std::string>("type") == "OpenCVCamera")
                sensor_list.emplace_back(makeSensor<OpenCVCamera>);
            else if(f.second.get<std::string>("type") == "RealSenseCamera")
                sensor_list.emplace_back(makeSensor<RealSenseCamera>);
            else if(f.second.get<std::string>("type") == "VimbaCamera")
                sensor_list.emplace_back(makeSensor<VimbaCamera>);
            else if(f.second.get<std::string>("type") == "RFEthernet")
                sensor_list.emplace_back(makeSensor<RFEthernet>);
            else if(f.second.get<std::string>("type") == "SerialPort")
                sensor_list.emplace_back(makeSensor<SerialPort>);
            else if(f.second.get<std::string>("type") == "MiniDSPMic")
                sensor_list.emplace_back(makeSensor<MiniDSPMic>);
            else
                std::cout << "Sensor attribute not recognized.\n";
            // Append initialization params
            local_configs.emplace_back(std::move(f));
            std::cout << f.second.get<std::string>("type") <<  "\n";
        }
        else if(at == GLOBAL)
        {
            global_config = f.second.get<std::string>("settings");
        }
        else
        {
            std::cout << "Houston we have a problem.\n";
        }
    }

    numSensors = sensor_list.size();
    for(auto i=0; i < numSensors; ++i)
    {
        sensors.emplace_back(std::move(sensor_list[i](local_configs[i], global_config)));
    }
}

void SensorStack::Acquire(double seconds)
{
    boost::barrier startAcq(numSensors);
    std::vector<boost::thread> threads_;
    for(auto& i : sensors)
    {
        boost::thread thread_(boost::bind(&(Sensor::AcquireSave), boost::ref(i), seconds, boost::ref(startAcq)));
        threads_.emplace_back(std::move(thread_)); //We have to move the thread.
    }
    for(auto& j : threads_)
    {
        j.join();
    }
}

SensorStack::~SensorStack()
{
    std::cout << "Acquisition Complete" << std::endl;
}
