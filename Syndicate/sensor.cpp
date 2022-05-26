#include "sensor.h"

size_t Sensor::numSensors(0);

Sensor::Sensor(std::unordered_map<std::string, std::any>& sample_config)
    : sensorName(std::any_cast<std::string>(sample_config["Sensor Name"])),
    rootPath(std::any_cast<std::string>(sample_config["Root Path"])),
    bufferSize(0),
    extrinsicMatrix(3, std::vector<double>(4, 0)),
    statusCode(HealthCode::OFFLINE), operatingCode(OpMode::NONE) 
{
    ++numSensors;
}

Sensor::~Sensor()
{
    std::cout << "Sensor " << sensorName << " is shutting down.\n";
}

HealthCode Sensor::checkHealthCode()
{
    boost::lock_guard<boost::mutex> guard(mtx_);
    return statusCode;
}

void Sensor::setHealthCode(HealthCode stat)
{
    boost::lock_guard<boost::mutex> guard(mtx_);
    statusCode = stat;
}