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

void SensorStack::ConcurrentAcquireSave(double seconds)
{
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
