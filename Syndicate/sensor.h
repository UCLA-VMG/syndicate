#include <iostream>
#include <vector>
#include <chrono>

template<class T>
struct Sensor{
    std::string sensorName;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> timeStamps;

    Sensor();

    virtual ~Sensor();

    virtual T AcquireData(size_t seconds);

    virtual T AcquireData(size_t frames);

    virtual T AcquireData();

    virtual void LogData();

    virtual void RecordTimeStamp();

    virtual void PrintStamps();

    



}