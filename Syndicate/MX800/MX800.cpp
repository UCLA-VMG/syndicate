#include "MX800.h"

MX800::MX800(std::unordered_map<std::string, std::any>& sample_config)
    : Sensor(sample_config), exe_path(std::any_cast<std::string>(sample_config["Exe Path"])),
    child_process_executable(exe_path, boost::process::std_out > _out, boost::process::std_in < _in)
{
    std::cout << "Executing Binaries for the MX800!\n";
}

void MX800::AcquireSave(double seconds) {
    _in << " " << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(seconds)));
    child_process_executable.terminate();
    std::cout << "MX800 Execution Complete!\n";
}

void MX800::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) {
    std::cout << "I am not defined yet.\n\n";
}

MX800::~MX800()
{
    std::cout << "MX800 execution is complete" << std::endl << std::endl;
}

void MX800::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    std::cout << "I am not defined yet.\n";
}

void MX800::ConcurrentSave()
{
    std::cout << "I am not defined yet.\n";
}