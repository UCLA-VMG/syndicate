#pragma once
#include "sensor.h"
#include "portaudio.h"

#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <any>

struct MiniDSPMic : public Sensor
{
    MiniDSPMic(std::unordered_map<std::string, std::any>& sample_config);
    ~MiniDSPMic(PaError err);

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);

    void _callback(const void* input_buffer, void* output_buffer,
    unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info,
    PaStreamCallbackFlags status_flags, void* userData);

    int _fs;
    int _channels;
    int _frame_index;
    int _max_frame_index;
    float* audio_samples_1;
    float* audio_samples_2;
    PaStream* record_stream;

};

// std::unique_ptr<Sensor> makeSimpleSensor(std::unordered_map<std::string, std::any>& sample_config);
