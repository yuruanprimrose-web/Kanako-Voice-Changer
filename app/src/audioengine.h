#pragma once

#include <portaudio.h>
#include "voiceprocessor.h"
#include "audiorecorder.h"
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
    void setVoiceTone(float pitchSemitones, float formantSemitones);
    void setMicrophoneGain(float gain);
    void setOutputGain(float gain);
    float inputLevel() const;
    bool startRecording(const QString &filePath, QString *errorMessage);
    void pauseRecording();
    void resumeRecording();
    void stopRecording();
    bool isRecording() const;
    bool isRecordingPaused() const;
    std::uint64_t droppedRecordingSamples() const;
    std::uint64_t acceptedRecordingSamples() const;
    float recordingPeakLevel() const;

private:
    static int audioCallback(
        const void *input,
        void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);

    VoiceProcessor processor_;
    AudioRecorder recorder_;
    double sampleRate_ = 48000.0;
    PaStream *stream_ = nullptr;
    std::atomic<float> inputLevel_{0.0F};
    std::atomic<float> microphoneGain_{1.0F};
    std::atomic<float> outputGain_{1.0F};
};
