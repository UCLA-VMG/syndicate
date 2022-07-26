#pragma once

#include "camera.h"
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <chrono>
#include <string>
#include <memory>
#include <any>
#include <iostream>

struct RealSenseCamera : public Syndicate::Camera 
{
    RealSenseCamera(std::unordered_map<std::string, std::any>& sample_config);
    ~RealSenseCamera();

    void AcquireSave(double seconds, boost::barrier& startBarrier);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    int cameraID;
    rs2::pipeline rs_pipe;
    rs2::colorizer color_map;
};

void metadata_to_csv(const rs2::frame& frm, const std::string& filename);
// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);