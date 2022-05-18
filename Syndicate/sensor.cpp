#include "sensor.h"

Sensor::Sensor(std::string sensor_name, std::string root_path)
    : sensorName(sensor_name),
    rootPath(root_path),
    bufferSize(0),
    extrinsicMatrix(3, std::vector<double>(4, 0))
{   
}

Sensor::~Sensor()
{
    std::cout << "Sensor" << sensorName << "is shutting down.\n";
}

