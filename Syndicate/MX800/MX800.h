#pragma once
#include "sensor.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <boost/process.hpp>

struct MX800 : public Sensor
{
    MX800(std::unordered_map<std::string, std::any>& sample_config);
    ~MX800();

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    std::string exe_path;

    boost::process::opstream _in;
    boost::process::ipstream _out;
    boost::process::child child_process_executable;
};

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
