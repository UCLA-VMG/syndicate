#include "sensorStack.h"

/// @Anirudh's idea for incorporating global + sensor settings = 
/// have each sensor take 2 ptree's as input
/// input A is global settings, input B is sensor settings
SensorStack::SensorStack(boost::property_tree::ptree tree)
{
    using boost::property_tree::ptree;
 
    std::vector<std::unique_ptr<Sensor>(*)(ptree::value_type&, std::string&)> sensor_list;
    std::vector<ptree::value_type> configs;
    std::string ATTR_SET("sensor");
    std::string SETTINGS("settings");
 
    BOOST_FOREACH(ptree::value_type & f, tree.get_child("sensor_stack")){
        std::string at = f.first;
        std::cout << at << " \n";
        if(at == ATTR_SET)
        {
            //Check if sensor is in list of defined sensors
            if(f.second.get<std::string>("type") == "mx800")
                sensor_list.emplace_back(makeSensor<MX800>);
            else if(f.second.get<std::string>("type") == "flir")
                sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
            else
                std::cout << "Sensor attribute not recognized.\n";
            //Append initialization params
            configs.emplace_back(std::move(f));
            std::cout << f.second.get<std::string>("type") <<  "\n";
        }
        else if(at == SETTINGS)
        {
            std::cout << at << " is not a sensor, I am saving at " << f.second.get<std::string>("save_path") << "\n";
            savePath = f.second.get<std::string>("save_path");
        }
        else
        {
            std::cout << "Houston we have a problem.\n";
        }
    }

    numSensors = sensor_list.size();
    for(auto i=0; i < numSensors; ++i)
    {
        sensors.emplace_back(std::move(sensor_list[i](configs[i], savePath)));
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

void SensorStack::AcquireBarrier(double seconds)
{
    boost::barrier frameBarrier(numSensors);
    std::vector<boost::thread> threads_;
    for(auto& i : sensors)
    {
        boost::thread thread_(boost::bind(&(Sensor::AcquireSaveBarrier), 
            boost::ref(i), seconds, boost::ref(frameBarrier)));
        threads_.emplace_back(std::move(thread_)); //We have to move the thread.
    }
    for(auto& j : threads_)
    {
        j.join();
    }
}

void SensorStack::ConcurrentAcquireSave(double seconds)
{
    boost::barrier frameBarrier(numSensors);
    std::vector<boost::thread> threads_acquire;
    for(auto& i : sensors)
    {
        boost::thread thread_(boost::bind(&(Sensor::ConcurrentAcquire), 
            boost::ref(i), seconds, boost::ref(frameBarrier)));
        threads_acquire.emplace_back(std::move(thread_)); //We have to move the thread.
    }
    std::vector<boost::thread> threads_save;
    for(auto& i : sensors)
    {
        boost::thread thread_(boost::bind(&(Sensor::ConcurrentSave), 
            boost::ref(i)));
        threads_save.emplace_back(std::move(thread_)); //We have to move the thread.
    }
    
    for(auto& j : threads_acquire)
    {
        j.join();
    }
    for(auto& j : threads_save)
    {
        j.join();
    }
}
