#include "camera.h"

namespace Syndicate
{
    Camera::Camera(std::unordered_map<std::string, std::any>& sample_config)
        : Sensor(sample_config),
        fps(std::any_cast<double>(sample_config["FPS"])),
        height(std::any_cast<int>(sample_config["Height"])),
        width(std::any_cast<int>(sample_config["Width"]))
    {
        std::cout << "Camera Cstr\n";
        if(sample_config.find("Camera Type") != sample_config.end())
        {
            cameraType = std::any_cast<std::string>(sample_config["Camera Type"]);
        }
    }
}