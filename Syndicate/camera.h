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

        virtual void AcquireSave(double seconds) = 0;
        virtual void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) = 0;


        //Operating Characteristics
        double fps;
        int height;
        int width;


    };
}