#include "camera.h"

namespace Syndicate
{
    Camera::Camera(ptree::value_type& tree, std::string& savePath)
        : Sensor(tree, savePath),  fps(tree.second.get<int>("fps")),
        height(tree.second.get<int>("height")),
        width(tree.second.get<int>("width"))

    {
        std::cout << "Camera Cstr\n";
        if(tree.second.find("type") != tree.second.not_found())
        {
            cameraType = std::any_cast<std::string>(tree.second.get<std::string>("type"));
        }
    }
}