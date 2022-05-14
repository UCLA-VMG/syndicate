#include "camera.h"

class camera : public sensor {
    float fps;
    int height, width, channels, num_frames, acquire_time, save_interval;
    string modality, mode, compression;
    string modality = "camera";

    public:
        void init(fps, height, width, mode, compression);
        void record(num_frames);
        void record_continuous(num_frames, acquire_time, save_interval, fps);
        void release();
}