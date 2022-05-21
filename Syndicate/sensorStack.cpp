#include "sensorStack.h"

SensorStack::SensorStack(
    std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)>& sensor_list,
    std::vector<std::unordered_map<std::string, std::any>>& configs)
    : frameBarrier(sensor_list.size())
{
    assert(sensor_list.size() == configs.size());

    int length = sensor_list.size();
    for(auto i=0; i < length; ++i)
    {
        sensors.emplace_back(std::move(sensor_list[i](configs[i])));
    }
}

void SensorStack::Acquire(double seconds)
{
    std::vector<boost::thread> threads_;
    for(auto& i : sensors)
    {
        boost::thread thread_(boost::bind(&(Sensor::AcquireSave), boost::ref(i), seconds));
        threads_.emplace_back(std::move(thread_)); //We have to move the thread.
    }
    for(auto& j : threads_)
    {
        j.join();
    }
}

void SensorStack::AcquireBarrier(double seconds)
{
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