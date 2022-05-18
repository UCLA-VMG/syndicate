#include <iostream>
#include <queue>
#include <vector>
#include <memory>

#include <thread>
#include <barrier>
#include <mutex>
#include <chrono>

enum HealthCode
{
    OFFLINE,
    BUSY,
    ONLINE
};

enum OpMode
{
    COLLECT,
    INFERENCE
};

struct Sensor{

    static size_t numSensors;
    std::shared_ptr<Sensor> primarySensor;

    //-------- Sensor Identifier and Operating Status/Mode/Characteristics ------------------------
    //Identifiers
    std::string sensorName;
    size_t sensorIdx;
    //Operating Status/Mode
    HealthCode statusCode;
    OpMode operatingCode;
    //Operating Characteristics
    double fps;
    
    //-------- Capture Data Structures : operatingCode == OpMode::COLLECT -------------
    std::vector<std::vector<double>> extrinsicMatrix;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> timeStamps;
    std::string rootPath;

    //-------- Inference Data Structures : operatingCode == OpMode::INFERENCE ---------
    std::queue<double> runningBuffer;
    std::queue<std::chrono::time_point<std::chrono::steady_clock>> runningTimestamps;
    const int bufferSize;

    //-------- General Functions -------------------------------------------------------
    Sensor(std::string sensor_name, std::string root_path);

    Sensor(int buffer_size);

    ~Sensor();

    // virtual double AcquireDataPoint();

    HealthCode HeartBeat();

    // virtual void LogData();

    // virtual void RecordTimeStamp();

    // virtual double AtomicAcquire();

    //-------- Capture Data Functions : operatingCode == OpMode::COLLECT -------------
    void AcquireSaveHsync(double seconds);

    void AcquireSaveHsync(long int frames);

    // void AcquireSaveSsync(double seconds, std::barrier<on_completion3>& frameBarrier);

    // void AcquireSaveSsync(long int frames, std::barrier* frameBarrier);

    void AcquireSave(double seconds);

    void AcquireSave(long int frames);

    std::vector<double> AcquireHsync(double seconds);

    std::vector<double> AcquireHsync(long int frames);

    // std::vector<double> AcquireSsync(double seconds, std::barrier& frameBarrier);

    // std::vector<double> AcquireSsync(long int frames, std::barrier& frameBarrier);

    std::vector<double> Acquire(double seconds);

    std::vector<double> Acquire(long int frames);

    //-------- Inference Functions : operatingCode == OpMode::INFERENCE ---------
    void ContiniousAcquire(size_t queue_size);

};