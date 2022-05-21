#pragma once

#include <iostream>
#include <queue>
#include <vector>
#include <memory>
#include <chrono>
#include <any>

#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>
#include <boost/atomic.hpp>

enum HealthCode
{
    OFFLINE,
    BUSY,
    ONLINE
};

enum OpMode
{
    NONE,
    COLLECT,
    INFERENCE
};

struct Sensor
{
    static size_t numSensors;
    std::shared_ptr<Sensor> primarySensor;

    //-------- Sensor Identifier and Operating Status/Mode/Characteristics ------------------------
    //Identifiers
    std::string sensorName;
    size_t sensorIdx;
    //Operating Status/Mode
    HealthCode statusCode;
    OpMode operatingCode;
    
    //-------- Capture Data Structures : operatingCode == OpMode::COLLECT -------------
    std::vector<std::vector<double>> extrinsicMatrix;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> timeStamps;
    std::string rootPath;

    //-------- Inference Data Structures : operatingCode == OpMode::INFERENCE ---------
    std::queue<double> runningBuffer;
    std::queue<std::chrono::time_point<std::chrono::steady_clock>> runningTimestamps;
    const int bufferSize;

    //-------- General Functions -------------------------------------------------------
    Sensor(std::unordered_map<std::string, std::any>& sample_config);

    Sensor(int buffer_size);

    ~Sensor();

    // virtual double AcquireDataPoint();

    HealthCode HeartBeat();

    // virtual void LogData();

    // virtual void RecordTimeStamp();

    //-------- Capture Data Functions : operatingCode == OpMode::COLLECT -------------
    // virtual void AcquireSaveHsync(double seconds) = 0;

    virtual void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) = 0;

    virtual void AcquireSave(double seconds) = 0;

    // std::vector<double> AcquireHsync(double seconds);

    // std::vector<double> AcquireHsync(long int frames);

    // std::vector<double> AcquireSsync(double seconds, boost::barrier& frameBarrier);

    // std::vector<double> AcquireSsync(long int frames, boost::barrier& frameBarrier);

    // std::vector<double> Acquire(double seconds);

    // std::vector<double> Acquire(long int frames);

    //-------- Inference Functions : operatingCode == OpMode::INFERENCE ---------
    void ContiniousAcquire(size_t queue_size);

};

template<class T>
std::unique_ptr<Sensor> makeSensor(std::unordered_map<std::string, std::any>& sample_config)
{
    return std::make_unique<T>(sample_config);
}