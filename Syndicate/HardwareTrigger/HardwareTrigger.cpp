#include "HardwareTrigger.h"

HardwareTrigger::HardwareTrigger(std::unordered_map<std::string, std::any>& sample_config)
    : Sensor(sample_config), 
    portName(std::any_cast<std::string>(sample_config["Port Name"])),
    pulseTime(std::any_cast<int>(sample_config["Pulse Time"])), 
    totalTime(std::any_cast<int>(sample_config["Total Time"]))
{
    _connected = false;

    _handler = CreateFileA(static_cast<LPCSTR>(portName.c_str()),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (_handler == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            std::cerr << "ERROR: Handle was not attached.Reason : " << portName << " not available\n";
        }
        else
        {
            std::cerr << "ERROR: " << "CreateFileA" << " failed with error code " << GetLastError() << "\n";
        }
    }
    else
    {
        DCB dcbSerialParameters = {0};

        if (!GetCommState(_handler, &dcbSerialParameters))
        {
            std::cerr << "Failed to get current serial parameters\n";
        }
        else
        {
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(_handler, &dcbSerialParameters))
            {
                std::cout << "ALERT: could not set serial port parameters\n";
                logFile << "ALERT: could not set serial port parameters\n";
            }
            else
            {
                _connected = true;
                PurgeComm(_handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }

    if (isConnected()) {
        std::cout  << std::endl << "Connection established at port " << portName << std::endl;
        logFile  << std::endl << "Connection established at port " << portName << std::endl;
    }
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool HardwareTrigger::writeHardwareTrigger(const char *buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(_handler, (void*) buffer, buf_size, &bytesSend, 0))
    {
        ClearCommError(_handler, &_errors, &_status);
        return false;
    }
    
    return true;
}

// Checking if serial port is connected
bool HardwareTrigger::isConnected()
{
    if (!ClearCommError(_handler, &_errors, &_status))
    {
        _connected = false;
    }

    return _connected;
}

void HardwareTrigger::AcquireSave(double seconds, boost::barrier& startBarrier) {
    // Better than recursion
    // Avoid stack overflows
    startBarrier.wait();
    std::string duration = std::to_string(static_cast<int>(seconds));
    std::cout << "Hardware Trigger Started" << std::endl;
    std::cout << duration.c_str() << std::endl;
    writeHardwareTrigger(std::to_string(static_cast<int>(seconds)).c_str(), duration.length() + 1);
}

void HardwareTrigger::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) {
    std::cout << "I am not defined yet.\n\n";
}

void HardwareTrigger::closeSerial()
{
    CloseHandle(_handler);
}

HardwareTrigger::~HardwareTrigger()
{
    if (_connected)
    {
        _connected = false;
        CloseHandle(_handler);
    }
    std::cout << "HardwareTrigger execution is complete" << std::endl << std::endl;
}

void HardwareTrigger::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    std::cout << "I am not defined yet.\n";
}

void HardwareTrigger::ConcurrentSave()
{
    std::cout << "I am not defined yet.\n";
}