#include <sensor.h>
#include <map>




struct SensorStack{

    SensorStack(std::vector<std::unique_ptr<Sensor>(*)(std::map<std::string, std::string>)>& sensors,
                std::vector<std::map<std::string, std::string>>& configs);
    ~SensorStack();

    ß

    void Acquire(double seconds);





}