#include "RespBelt.h"

RespBelt::RespBelt(ptree::value_type& sensor_settings, ptree::value_type& global_settings)
    : Sensor(sensor_settings, global_settings), 
    portName(sensor_settings.second.get<std::string>("port_name")),
    pulseTime(static_cast<unsigned int>(sensor_settings.second.get<double>("pulse_time")))
{
    std::cout<<portName << " RespBelt\n";
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
            std::cerr << "ERROR!!! Error code: "<< std::to_string(GetLastError()) << "\n";
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
            dcbSerialParameters.BaudRate = CBR_19200;
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
                // Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }

    if (isConnected()) {
        std::cout  << std::endl << "Connection established at port " << portName << std::endl;
        logFile  << std::endl << "Connection established at port " << portName << std::endl;
    }
}

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int RespBelt::readRespBelt(const char *buffer, unsigned int buf_size)
{
    DWORD bytesRead{};
    unsigned int toRead = 0;

    ClearCommError(_handler, &_errors, &_status);

    if (_status.cbInQue > 0)
    {
        if (_status.cbInQue > buf_size)
        {
            toRead = buf_size;
        }
        else
        {
            toRead = _status.cbInQue;
        }
    }

    memset((void*) buffer, 0, buf_size);

    if (ReadFile(_handler, (void*) buffer, toRead, &bytesRead, NULL))
    {
        RecordTimeStamp();
        return bytesRead;
    }

    return 0;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool RespBelt::writeRespBelt(const char *buffer, unsigned int buf_size)
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
bool RespBelt::isConnected()
{
    if (!ClearCommError(_handler, &_errors, &_status))
    {
        _connected = false;
    }

    return _connected;
}


void RespBelt::signalWriteRead(unsigned int delayTime, std::string command)
{
    // Keep the volatile qualifier and logging. Might not work without them
    volatile int write_status, read_status;
    write_status = writeRespBelt(command.c_str(), MAX_DATA_LENGTH);
    read_status  = readRespBelt(incomingData, MAX_DATA_LENGTH);
    if (delayTime)
        std::this_thread::sleep_for(std::chrono::seconds(delayTime));
    logFile << incomingData << std::endl;
}


void RespBelt::AcquireSave(double seconds, boost::barrier& startBarrier) {
    // Better than recursion
    // Avoid stack overflows
    volatile int read_status, write_status;
    startBarrier.wait();

    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();

    while((static_cast<double>((end-start).count())/1'000'000'000) < seconds+5) // 5 seconds for safety
    {
        read_status  = readRespBelt(incomingData, MAX_DATA_LENGTH);
        std::this_thread::sleep_for(std::chrono::milliseconds(pulseTime));
        // logFile << "NEW PACKET" << std::endl;
        logFile << incomingData;
        end = std::chrono::steady_clock::now();
    }

    write_status = writeRespBelt("\n", MAX_DATA_LENGTH);

    SaveTimeStamps();
    std::cout << "Serial execution complete" << std::endl;
}


void RespBelt::closeSerial()
{
    CloseHandle(_handler);
}

RespBelt::~RespBelt()
{
    if (_connected)
    {
        _connected = false;
        CloseHandle(_handler);
    }
    std::cout << "RespBelt execution is complete" << std::endl << std::endl;
}
