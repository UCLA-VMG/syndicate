#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

enum HealthCode
{
    OFFLINE,
    BUSY,
    ONLINE
}

enum OpMode
{
    COLLECT,
    INFERENCE
}

template<class T>
struct Sensor{

    static size_t numSensors{0};
    static std::shared_ptr<Sensor> primarySensor;

    //-------- Sensor Identifier and Operating Status/Mode/Characteristics ------------------------
    //Identifiers
    std::string sensorName;
    size_t sensorIdx;
    //Operating Status/Mode
    HealthCode::statusCode;
    OpMode::operatingCode;
    //Operating Characteristics
    double fps;
    
    //-------- Capture Data Structures : operatingCode == OpMode::COLLECT -------------
    std::vector<std::vector<double>> extrinsicMatrix(3, std::vector<double>(4, 0));
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> timeStamps;
    std::string rootPath;

    //-------- Inference Data Structures : operatingCode == OpMode::INFERENCE ---------
    std::queue<T> runningBuffer;
    std::queue<std::chrono::time_point<std::chrono::steady_clock> runningTimestamps;
    const int bufferSize;

    //-------- General Functions -------------------------------------------------------
    Sensor(std::string sensor_name, std::string root_path);

    Sensor(int buffer_size);

    ~Sensor();

    virtual T AcquireDataPoint();

    HealthCode HeartBeat();

    virtual void LogData();

    virtual void RecordTimeStamp();

    virtual T AtomicAcquire();

    //-------- Capture Data Functions : operatingCode == OpMode::COLLECT -------------
    void AcquireSaveHsync(double seconds);

    void AcquireSaveHsync(long int frames);

    void AcquireSaveSsync(double seconds, std::barrier& frameBarrier);

    void AcquireSaveSsync(long int frames, std::barrier& frameBarrier);

    void AcquireSave(double seconds);

    void AcquireSave(long int frames);

    std::vector<T> AcquireHsync(double seconds);

    std::vector<T> AcquireHsync(long int frames);

    std::vector<T> AcquireSsync(double seconds, std::barrier& frameBarrier);

    std::vector<T> AcquireSsync(long int frames, std::barrier& frameBarrier);

    std::vector<T> Acquire(double seconds);

    std::vector<T> Acquire(long int frames);

    //-------- Inference Functions : operatingCode == OpMode::INFERENCE ---------
    void ContiniousAcquire(size_t queue_size);

}