#include "miniDSPMic.h"

MiniDSPMic::MiniDSPMic(std::unordered_map<std::string, std::any>& sample_config)
    : Sensor(sample_config), _fs(std::any_cast<std::int>(sample_config["FS"])), _channels(std::any_cast<std::int>(sample_config["Channels"]))
{
    PaStreamParameters input_params;
    PaError err = paNoError;

    // Initialize Port Audio
    err = Pa_Initialize();
    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        return;
    }
    std::cout << "Initialized Port Audio." << std::endl << std::endl;

    // Get the default input device
    input_params.device = Pa_GetDefaultInputDevice()

    if (input_params.device == paNoDevice)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        return;
    }
    const PaDeviceInfo* device_info = Pa_GetDeviceInfo(input_params.device);

    std::cout << "Name                  = " << deviceInfo->name  << std::endl;
    std::cout << "Host API              = " << Pa_GetHostApiInfo(deviceInfo->hostApi)->name  << std::endl;
    std::cout << "Max inputs            = " << deviceInfo->maxInputChannels  << std::endl;
    std::cout << "Max outputs           = " << deviceInfo->maxOutputChannels  << std::endl;
    std::cout << "Default sampling rate = " << deviceInfo->defaultSampleRate       << std::endl;

    input_params.channelCount = _channels;                    /* stereo input */
    input_params.sampleFormat = paFloat32;
    input_params.suggestedLatency = Pa_GetDeviceInfo(input_params.device)->defaultLowInputLatency;
    input_params.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&record_stream, &input_params, NULL, _fs,
                        std::any_cast<std::int>(sample_config["Frames per Buffer"]), paClipOff, _callback, nullptr);

    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        return;
    }

}

void MiniDSPMic::MiniDSPMicAcquireSave(double seconds) {
    int total_frames, number_samples, number_bytes;

    _max_frame_index = total_frames = seconds * _fs; /* Record for a few seconds. */
    _frame_index = 0;
    number_samples = total_frames * _channels;
    number_bytes = number_samples * sizeof(float);
    audio_samples_1 = (float*)malloc(number_bytes); /* From now on, recordedSamples is initialised. */

    err = Pa_StartStream(record_stream);
    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        return;
    }
    std::cout << "---------------- Now recording!! Please speak into the microphone. ----------------" << std::endl << std::endl; 

    while ((err = Pa_IsStreamActive(record_stream)) == 1)
    {
        Pa_Sleep(1000);
        std::cout << "Frame = " << _frame_index <<std::endl; 
        fflush(stdout);
    }
    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        return;
    }

    err = Pa_CloseStream(record_stream);

    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        return;
    }

    // Save the recording 
    FILE* fid;
    fid = fopen("recorded.raw", "wb");
    if (fid == NULL)
    {
        std::cout << "Could not open file." << std::endl;
    }
    else
    {
        fwrite(audio_samples_1, _channels * sizeof(float), total_frames, fid);
        fclose(fid);
        std::cout << "Wrote data to 'recorded.raw'" << std::endl;
    }
}

void MiniDSPMic::MiniDSPMicAcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) {
    std::cout << "TBD" << std::endl << std::endl;
}

int MiniDSPMic::_callback(const void* input_buffer, void* output_buffer,
    unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info,
    PaStreamCallbackFlags status_flags, void* user_data) {

    const float* read_ptr = (const float*)input_buffer;
    // float* wptr = &data->recordedSamples[data->frameIndex * _channels];
    float* write_ptr = &audio_samples_1[_frame_index * _channels];
    long frames_to_calc;
    long i;
    int finished;
    unsigned long frames_remaining = _max_frame_index - _frame_index;

    /* Prevent unused variable warnings. */
    (void)output_buffer;
    (void)time_info;
    (void)status_flags;
    (void)user_data;

    if (frames_remaining < frames_per_buffer) {
        frames_to_calc = frames_remaining;
        finished = paComplete;
    }
    else {
        frames_to_calc = frames_per_buffer;
        finished = paContinue;
    }

    if (input_buffer == NULL) {
        for (i = 0; i < frames_to_calc; i++) {
            for (int j = 0; j < _channels; j++) {
                *write_ptr++ = 0.0f;
            }
        }
    }
    else {
        for (i = 0; i < frames_to_calc; i++) {
            for (int j = 0; j < _channels; j++) {
                *write_ptr++ = *read_ptr++;
            }
        }
    }
    _frame_index += frames_to_calc;
    return finished;
}

MiniDSPMic::~MiniDSPMic() {
    Pa_Terminate();
    if (audio_samples_1) free(audio_samples_1);
    if (audio_samples_2) free(audio_samples_2);
    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    }
}