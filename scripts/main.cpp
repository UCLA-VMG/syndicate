#include <iostream>
#include <Spinnaker.h>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <unordered_map>

#include "sensor.h"
#include "simpleSensor.h"
#include "sensorStack.h"
#include "SpinnakerCamera.h"

boost::mutex io_mutex;

void thread_fun(boost::barrier& cur_barier, boost::atomic<int>& current)
{
    ++current;
    cur_barier.wait();
    boost::lock_guard<boost::mutex> locker(io_mutex);
    std::cout << current << std::endl;
}

int main(int, char**) {
    std::cout << "Hello, world!\n";
    
    //1. Create Configurations
    std::unordered_map<std::string, std::any> sample_config = {
        {"FPS", 60},
        {"Sensor Name", std::string("Simp1")},
        {"Root Path", std::string("/rand/rand/...")}
    };
    std::unordered_map<std::string, std::any> sample_config2 = {
        {"FPS", 60},
        {"Sensor Name", std::string("Simp2")},
        {"Root Path", std::string("/rand/rand/...")}
    };
    std::unordered_map<std::string, std::any> nir_config = {
        {"Camera ID", std::string("21190637")},
        {"FPS", 30},{"Height", 640}, {"Width", 640},
        {"Sensor Name", std::string("NIR Camera")},
        {"Root Path", std::string("/rand/rand/...")}
    };
    std::unordered_map<std::string, std::any> polarized_config = {
        {"Camera ID", std::string("19224369")},
        {"FPS", 30},{"Height", 640}, {"Width", 640},
        {"Sensor Name", std::string("Polarized Camera")},
        {"Root Path", std::string("/rand/rand/...")}
    };
    
    //2. Add Configurations and Factory Generator Functions into std::vectors
    std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)> sensor_list;
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    // sensor_list.emplace_back(makeSensor<SimpleSensor>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    sensor_list.emplace_back(makeSensor<SpinnakerCamera>);
    std::vector<std::unordered_map<std::string, std::any>> configs{nir_config, polarized_config};//sample_config, sample_config2, nir_config, polarized_config};
    
    //3. Initialize Sensor Stack
    SensorStack mainStack(sensor_list, configs);
    
    // //4. Acquire Data

    //4.1 Asynchronously Acquire Data
    std::cout << "\n\n\nAsyn Capture \n";
    mainStack.Acquire(20);
    // //4.2 Barrier Sync Acquire Data
    std::cout << "\n\n\n Barrier Sync Capture\n";
    mainStack.AcquireBarrier(20);

    //Test base class calling derived class func
    // SimpleSensor a(sample_config);
    // Sensor* b = nullptr;
    // b = &a;
    // b->AcquireSave(10);
}