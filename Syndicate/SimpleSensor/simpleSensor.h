#include "sensor.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>

struct SimpleSensor : public Sensor 
{
    SimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
    // ~SimpleSensor();

    void printPls();

    std::chrono::duration<double> AtomicAcquire();

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);

    double fps;

};

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
