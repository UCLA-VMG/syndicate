#include <string>
#include "sensor.h"

class rf : public sensor {
    float fps;
    int channels, num_frames, acquire_time, save_interval;
    string mode, compression;
    string modality = "rf";

    public:
        void init(fps, height, width, mode, compression);
        void record(num_frames, acquire_time, fps);
        void record_continuous(num_frames, acquire_time, save_interval, fps);
        void release();
}