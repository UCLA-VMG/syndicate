#include "MX800.h"

MX800::MX800(std::unordered_map<std::string, std::any>& sample_config)
    : Sensor(sample_config), exe_path(std::any_cast<std::string>(sample_config["Exe Path"])),
    child_process_executable(exe_path, boost::process::std_out > _out, boost::process::std_in < _in),
    list_of_files({
                    "NOM_ECG_ELEC_POTL_AVRWaveExport.csv",
                    "NOM_ECG_ELEC_POTL_MCLWaveExport.csv",
                    "NOM_ECG_ELEC_POTL_VWaveExport.csv",
                    "NOM_ECG_ELEC_POTL_IIWaveExport.csv",
                    "NOM_ECG_ELEC_POTL_AVFWaveExport.csv",
                    "NOM_PLETHWaveExport.csv",
                    "NOM_RESPWaveExport.csv",
                    "MPDataExport.csv",
                    "MPrawoutput.txt"
                })
{
    std::cout << "The MX800 is ready !\n";
}

void MX800::AcquireSave(double seconds, boost::barrier& startBarrier) {
    _in << " " << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(1)));
    startBarrier.wait();
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(seconds+1)));
    child_process_executable.terminate();
    std::cout << "MX800 Execution Complete!\n";

    for (const std::string& file :  list_of_files)
    {
        try
        {
            std::filesystem::rename(file, rootPath + '/' + file);
        }
        catch (std::filesystem::filesystem_error& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    std::cout << "MX800 Files have been moved!\n";
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