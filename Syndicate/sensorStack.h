#include <sensor.h>
// #include "simpleSensor.h"
// #include "SpinnakerCamera.h"
// #include "VimbaCamera.h"
// #include "SerialPort.h"
// #include "OpenCVCamera.h"
// #include "RFEthernet.h"
// #include "MiniDSPMic.h"
#include "MX800.h"
// #include "RealSenseCamera.h"

#include <unordered_map>
#include <any>

#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>
#include <boost/atomic.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>



struct SensorStack{

    std::vector<std::unique_ptr<Sensor>> sensors;

    std::string savePath;
    size_t numSensors;

    SensorStack(boost::property_tree::ptree tree);
    // ~SensorStack();

    void ConcurrentAcquireSave(double seconds);

    void Acquire(double seconds);

    void AcquireBarrier(double seconds);



};