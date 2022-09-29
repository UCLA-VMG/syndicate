#pragma once

#include "sensor.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>

namespace Syndicate
{
    struct Camera : public Sensor 
    {
        Camera(std::unordered_map<std::string, std::any>& sample_config);
        // ~Camera();

        virtual void AcquireSave(double seconds, boost::barrier& startBarrier) = 0;
        virtual void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) = 0;

        //Operating Characteristics
        std::string cameraType;
        double fps;
        double height;
        double width;
        // std::string pixelFormat;
    
        // int bin_size;
        // double exposure_compensation;
        // double exposure_time;
        // double gain;
        // double black_level;
    };
}