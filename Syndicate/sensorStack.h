#include <sensor.h>
#include <unordered_map>
#include <any>

#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>
#include <boost/atomic.hpp>



struct SensorStack{

    std::vector<std::unique_ptr<Sensor>> sensors;
    boost::barrier frameBarrier;

    SensorStack(std::vector<std::unique_ptr<Sensor>(*)(std::unordered_map<std::string, std::any>&)>& sensor_list,
                std::vector<std::unordered_map<std::string, std::any>>& configs);
    // ~SensorStack();

    void Acquire(double seconds);

    void AcquireBarrier(double seconds);

};