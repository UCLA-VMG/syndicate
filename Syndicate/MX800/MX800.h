#pragma once
#include "sensor.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <filesystem>
#include <vector>
#include <boost/process.hpp>

struct MX800 : public Sensor
{
    MX800(std::unordered_map<std::string, std::any>& sample_config);
    ~MX800();

    void AcquireSave(double seconds, boost::barrier& startBarrier);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    std::string exe_path;
    const std::vector<std::string> list_of_files;

    boost::process::opstream _in;
    boost::process::ipstream _out;
    boost::process::child child_process_executable;
};

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
