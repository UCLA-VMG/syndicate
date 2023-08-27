#include "sensorStack.h"

SensorStack::SensorStack(boost::property_tree::ptree tree)
{
    // 1. Extract XML File and place into Ptree objects
    using boost::property_tree::ptree;
    std::vector<std::unique_ptr<Sensor>(*)(ptree::value_type&, ptree::value_type&)> sensor_list;
    std::string ATTR_SET("sensor");
    std::string GLOBAL("global");
    BOOST_FOREACH(ptree::value_type & f, tree.get_child("sensor_stack")){
        std::string at = f.first;
        if(at == ATTR_SET)
        {
            // Check if sensor is in list of defined sensors
            if(f.second.get<std::string>("type") == "MX800")
                sensor_list.emplace_back(makeSensor<MX800>);
            else if(f.second.get<std::string>("type") == "SpinnakerCamera")
                sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
            else if(f.second.get<std::string>("type") == "OpenCVCamera")
                sensor_list.emplace_back(makeSensor<OpenCVCamera>);
            // else if(f.second.get<std::string>("type") == "RealSenseCamera")
            //     sensor_list.emplace_back(makeSensor<RealSenseCamera>);
            // else if(f.second.get<std::string>("type") == "VimbaCamera")
            //     sensor_list.emplace_back(makeSensor<VimbaCamera>);
            else if(f.second.get<std::string>("type") == "RFEthernet")
                sensor_list.emplace_back(makeSensor<RFEthernet>);
            else if(f.second.get<std::string>("type") == "HardwareTrigger")
                sensor_list.emplace_back(makeSensor<HardwareTrigger>);
            else if(f.second.get<std::string>("type") == "SerialPort")
                sensor_list.emplace_back(makeSensor<SerialPort>);
            else if(f.second.get<std::string>("type") == "RespBelt")
                sensor_list.emplace_back(makeSensor<RespBelt>);
            else if(f.second.get<std::string>("type") == "MiniDSPMic")
                sensor_list.emplace_back(makeSensor<MiniDSPMic>);
            else
                std::cout << "Sensor attribute not recognized.\n";
            // Append initialization params
            local_configs.emplace_back(std::move(f));
        }
        else if(at == GLOBAL)
        {
            global_config.emplace_back(std::move(f));
        }
        else
        {
            std::cout << "Houston we have a problem.\n" << f.second.get<std::string>("type") << " is causing issues.\n";
        }
    }
    // 2. Create Directory. If folder with prefix already exists, increment suffix value. (1_1->1_2)
    std::filesystem::path rootPath(global_config[0].second.get<std::string>("save_path"));
    std::string unique_prefix(global_config[0].second.get<std::string>("prefix_id")+"_");
    size_t latest_suffix = 0;
    for (const auto & entry : std::filesystem::directory_iterator(rootPath)){
        std::string temp_entry = entry.path().filename().string();
        std::size_t found = temp_entry.find(unique_prefix);
        if (found!=std::string::npos and found == 0){ //unique_suffix exists
            size_t candidate_suffix = std::stoi(temp_entry.substr(unique_prefix.length(), temp_entry.length()-unique_prefix.length()));
            if(candidate_suffix > latest_suffix)
                latest_suffix = candidate_suffix;
        }
    }
    if(latest_suffix == 0){latest_suffix = 1;}
    else{latest_suffix += 1;}
    std::string newRootPath(global_config[0].second.get<std::string>("save_path") + unique_prefix + std::to_string(static_cast<int>(latest_suffix)) + "/");
    std::filesystem::create_directory(newRootPath);
    std::cout << "Saving Data at: " << newRootPath << "\n";
    global_config[0].second.put("save_path", newRootPath);
    // 3. Initialize Sensors with their constructors
    numSensors = sensor_list.size();
    for(auto i=0; i < numSensors; ++i)
    {
        sensors.emplace_back(std::move(sensor_list[i](local_configs[i], global_config[0])));
    }
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
