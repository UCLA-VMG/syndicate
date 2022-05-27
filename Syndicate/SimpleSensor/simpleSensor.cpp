#include "simpleSensor.h"

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config)
// {
//     return std::make_unique<SimpleSensor>(sample_config);
// }

SimpleSensor::SimpleSensor(std::unordered_map<std::string, std::any>& sample_config)
    : Sensor(sample_config),  fps(std::any_cast<int>(sample_config["FPS"]))
{
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

void SimpleSensor::AcquireSave(double seconds)
{
    int num_iters = static_cast<int>(seconds*fps);
    std::vector<std::chrono::duration<double>> data;
    auto start1 = std::chrono::steady_clock::now();
    for(auto i = 0; i < num_iters; ++i)
    {
        auto start = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        auto end = std::chrono::steady_clock::now();
        data.emplace_back(end-start);
        std::cout << sensorName << " " << (end-start).count() << " " << data.size() << "\n";
    }
    auto end1 = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for " << sensorName << (end1-start1).count() << "\n";
}

void SimpleSensor::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier)
{
    int num_iters = static_cast<int>(seconds*fps);
    std::vector<std::chrono::duration<double>> data;
    auto start1 = std::chrono::steady_clock::now();
    for(auto i = 0; i < num_iters; ++i)
    {
        frameBarrier.wait();
        auto start = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        auto end = std::chrono::steady_clock::now();
        data.emplace_back(end-start);
        std::cout << sensorName << " " << (end-start).count() << " " << data.size() << "\n";
    }
    auto end1 = std::chrono::steady_clock::now();
    std::cout <<"Time Taken for " << sensorName << (end1-start1).count() << "\n";
}
