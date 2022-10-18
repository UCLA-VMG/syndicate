#include "sensorStack.h"

SensorStack::SensorStack(boost::property_tree::ptree tree)
{
    using boost::property_tree::ptree;
 
    std::vector<std::unique_ptr<Sensor>(*)(ptree::value_type&, ptree::value_type&)> sensor_list;
    // ptree::value_type global_config;
    std::string ATTR_SET("sensor");
    std::string GLOBAL("global");
 
    BOOST_FOREACH(ptree::value_type & f, tree.get_child("sensor_stack")){
        std::string at = f.first;
        std::cout << at << " \n";
        if(at == ATTR_SET)
        {
            // Check if sensor is in list of defined sensors
            if(f.second.get<std::string>("type") == "SpinnakerCamera")
                sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
            else if(f.second.get<std::string>("type") == "OpenCVCamera")
                sensor_list.emplace_back(makeSensor<OpenCVCamera>);
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
            global_config.emplace_back(std::move(f));
        }
        else
        {
            std::cout << "Houston we have a problem.\n";
        }
    }

    numSensors = sensor_list.size();
    for(auto i=0; i < numSensors; ++i)
    {
        std::cout << "hi1\n";
        sensors.emplace_back(std::move(sensor_list[i](local_configs[i], global_config[0])));
        std::cout << "hi2\n";
    }
    std::cout << "hi3\n";
}

void SensorStack::Acquire()
{
    double seconds = global_config[0].second.get<double>("record_time");
    boost::barrier startAcq(numSensors);
    std::vector<boost::thread> threads_;
    for(auto& i : sensors)
    {
        boost::thread thread_(boost::bind(&(Sensor::AcquireSave), boost::ref(i), seconds, boost::ref(startAcq)));
        threads_.emplace_back(std::move(thread_)); // We have to move the thread.
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
