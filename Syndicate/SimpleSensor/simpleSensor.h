#include "sensor.h"

#include <chrono>
#include <thread>
#include <string>

struct SimpleSensor : public Sensor<std::chrono::duration<double>> 
{
    SimpleSensor(std::string sensor_name, std::string root_path);
    // ~SimpleSensor();

    void printPls();

    std::chrono::duration<double> AtomicAcquire();

};