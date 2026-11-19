#pragma once

#include <portaudio.h>
#include "voiceprocessor.h"
#include <QString>
#include <atomic>

class AudioEngine final
{
public:
    AudioEngine() = default;
    ~AudioEngine();

    AudioEngine(const AudioEngine &) = delete;
    AudioEngine &operator=(const AudioEngine &) = delete;

    bool start(PaDeviceIndex inputDevice, PaDeviceIndex outputDevice, QString *errorMessage);
    void stop();
    bool isRunning() const;
    void setPreset(VoiceProcessor::Preset preset);
    void setMicrophoneGain(float gain);
    void setOutputGain(float gain);
    float inputLevel() const;

private:
    static int audioCallback(
        const void *input,
        void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);

    VoiceProcessor processor_;
    PaStream *stream_ = nullptr;
    std::atomic<float> inputLevel_{0.0F};
    std::atomic<float> microphoneGain_{1.5F};
    std::atomic<float> outputGain_{1.5F};
};
