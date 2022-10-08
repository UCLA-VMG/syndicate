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
    RealSenseCamera(ptree::value_type& sensor_settings, ptree::value_type& global_settings);
    ~RealSenseCamera();

    void AcquireSave(double seconds, boost::barrier& startBarrier);

    int cameraID;
    rs2::pipeline rs_pipe;
    rs2::colorizer color_map;
};

void metadata_to_csv(const rs2::frame& frm, const std::string& filename);
