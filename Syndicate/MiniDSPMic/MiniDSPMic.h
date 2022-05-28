#pragma once
#include "sensor.h"
#include "portaudio.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>

typedef struct
{
    int     fs;
    int     channels;
    int     frame_index;  /* Index into sample array. */
    int     max_frame_index;
    float* audio_samples_1;
    float* audio_samples_2;
}
paRecordData;

struct MiniDSPMic : public Sensor
{
    MiniDSPMic(std::unordered_map<std::string, std::any>& sample_config);
    ~MiniDSPMic();

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    // int _fs;
    // int _channels;
    paRecordData _data;
    PaStream* record_stream;

    
};

static int _callback(const void* input_buffer, void* output_buffer,
    unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info,
    PaStreamCallbackFlags status_flags, void* userData);

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
