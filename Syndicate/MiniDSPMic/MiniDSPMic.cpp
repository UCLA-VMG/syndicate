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
        ~MiniDSPMic();
    }
    std::cout << "Initialized Port Audio." << std::endl << std::endl;

    // Get the default input device
    input_params.device = Pa_GetDefaultInputDevice()

    if (input_params.device == paNoDevice)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        ~MiniDSPMic();
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
                        std::any_cast<std::int>(sample_config["Frames per Buffer"]), paClipOff, _callback, NULL);

    if (err != paNoError)
    {
        fprintf(stderr, "An error occurred while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        ~MiniDSPMic();
    }

}

void AcquireSave(double seconds) {
    cout << "TBD";
}

void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier) {
    cout << "TBD";
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