#include "SerialPort.h"

SerialPort::SerialPort(std::unordered_map<std::string, std::any>& sample_config)
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
            std::cerr << "ERROR!!!\n";
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

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int SerialPort::readSerialPort(const char *buffer, unsigned int buf_size)
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
        return bytesRead;
    }

    return 0;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool SerialPort::writeSerialPort(const char *buffer, unsigned int buf_size)
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
bool SerialPort::isConnected()
{
    if (!ClearCommError(_handler, &_errors, &_status))
    {
        _connected = false;
    }

    return _connected;
}

void SerialPort::runBarker13()
{
    std::string str_cmd;
    int remainingTime = totalTime - (13 * pulseTime);
    // Add assertion (2 * 13 * pulseTime) < totalTime
    int idx = 0;
    int seq[] = {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1};
    RecordTimeStamp();
    for(int idx=0; idx < 13 ; idx++) {
        if (seq[idx] == 1)
        {
            str_cmd = "<ON>";
            signalWriteRead(pulseTime, str_cmd);
        }
        else
        {
            str_cmd = "<OFF>";
            signalWriteRead(pulseTime, str_cmd);
        }
    }    
    str_cmd = "<OFF>";
    signalWriteRead(0, str_cmd);
    std::this_thread::sleep_for(std::chrono::seconds(remainingTime));
}

void SerialPort::signalWriteRead(unsigned int dealyTime, std::string command)
{
    // Keep the voltile qualifier and logging. Might not work without them
    volatile int write_status, read_status;
    write_status = writeSerialPort(command.c_str(), MAX_DATA_LENGTH);
    read_status  = readSerialPort(incomingData, MAX_DATA_LENGTH);
    if (dealyTime)
        std::this_thread::sleep_for(std::chrono::seconds(dealyTime));
    logFile << incomingData << std::endl;
}

void SerialPort::AcquireSave(double seconds, boost::barrier& startBarrier) {
    // Better than recusion
    // Avoid stack overflows
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();

    while((static_cast<double>((end-start).count())/1'000'000'000) < seconds)
    {
        startBarrier.wait();
        runBarker13();
        end = std::chrono::steady_clock::now();
    }
    SaveTimeStamps();
    std::cout << "Serial execution complete" << std::endl;
}

void SerialPort::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) {
    std::cout << "I am not defined yet.\n\n";
}

void SerialPort::closeSerial()
{
    CloseHandle(_handler);
}

SerialPort::~SerialPort()
{
    if (_connected)
    {
        _connected = false;
        CloseHandle(_handler);
    }
    std::cout << "SerialPort execution is complete" << std::endl << std::endl;
}

void SerialPort::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    std::cout << "I am not defined yet.\n";
}

void SerialPort::ConcurrentSave()
{
    std::cout << "I am not defined yet.\n";
}