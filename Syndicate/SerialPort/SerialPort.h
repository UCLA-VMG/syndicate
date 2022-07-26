#pragma once
#include "sensor.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>
#include <vector>
#include <iostream>
#include <windows.h>
#include <filesystem>

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

struct SerialPort : public Sensor
{
    explicit SerialPort(std::unordered_map<std::string, std::any>& sample_config);
    ~SerialPort();

    void AcquireSave(double seconds, boost::barrier& startBarrier);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    int readSerialPort(const char *buffer, unsigned int buf_size);
    bool writeSerialPort(const char *buffer, unsigned int buf_size);
    bool isConnected();
    void closeSerial();
    void signalWriteRead(unsigned int delayTime, std::string command);
    void runBarker13();

    char incomingData[MAX_DATA_LENGTH];
    std::string portName;
    unsigned int pulseTime, totalTime;

    HANDLE _handler;
    bool _connected;
    COMSTAT _status;
    DWORD _errors;
};

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
