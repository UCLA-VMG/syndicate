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
    MX800(ptree::value_type& sensor_settings, ptree::value_type& global_settings);
    ~MX800();

    void AcquireSave(double seconds, boost::barrier& startBarrier);

    std::string exe_path;
    const std::vector<std::string> list_of_files;

    boost::process::opstream _in;
    boost::process::ipstream _out;
    boost::process::child child_process_executable;
};
