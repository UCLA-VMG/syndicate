#include "MX800.h"

MX800::MX800(ptree::value_type& sensor_settings, ptree::value_type& global_settings)
    : Sensor(sensor_settings, global_settings), exe_path(sensor_settings.second.get<std::string>("exe_path")),
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
    // The command below runs the executable needs to operate the Philips MX800 monitor.
    _in << " " << std::endl;
    // The MX800 is started before all the other sensors. Hence, the startBarrier is called after the executable is run.
    std::this_thread::sleep_for(std::chrono::seconds(6));
    // Wait for the other sensors to get ready,
    startBarrier.wait();
    // We need to wait for a little longer after the data capture to make sure we have all the necessary ground truth signals.
    // The timestamps of the GT and the modalities are used to align the signals post capture.
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(seconds+3)));
    child_process_executable.terminate();
    std::cout << "MX800 Execution Complete!\n";

    for (const std::string& file :  list_of_files)
    {
        try
        {
            std::filesystem::rename(file, rootPath + file);
        }
        catch (std::filesystem::filesystem_error& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    std::cout << "MX800 Files have been moved!\n";
}

MX800::~MX800()
{
    std::cout << "MX800 execution is complete" << std::endl << std::endl;
}
