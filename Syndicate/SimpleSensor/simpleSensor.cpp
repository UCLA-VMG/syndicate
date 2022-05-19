#include "simpleSensor.h"

SimpleSensor::SimpleSensor(std::string sensor_name, std::string root_path)
    : Sensor<std::chrono::duration<double>>(sensor_name, root_path)
{
    fps = 30;
}

void SimpleSensor::printPls()
{
    std::cout <<"hi\n";
}

std::chrono::duration<double> SimpleSensor::AtomicAcquire()
{
    int a = static_cast<int>(900/fps);
    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    auto end = std::chrono::steady_clock::now();
    return end-start;
}