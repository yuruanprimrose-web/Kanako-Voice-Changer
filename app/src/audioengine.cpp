#include "audioengine.h"

#include <algorithm>
#include <array>
#include <cmath>

AudioEngine::~AudioEngine()
{
    stop();
}

bool AudioEngine::start(PaDeviceIndex inputDevice, PaDeviceIndex outputDevice, QString *errorMessage)
{
    stop();
    inputLevel_.store(0.0F, std::memory_order_relaxed);

    const PaDeviceInfo *inputInfo = Pa_GetDeviceInfo(inputDevice);
    const PaDeviceInfo *outputInfo = Pa_GetDeviceInfo(outputDevice);
    if (inputInfo == nullptr || outputInfo == nullptr) {
        if (errorMessage) *errorMessage = "The selected microphone or output device is no longer available.";
        return false;
    }

    PaStreamParameters inputParameters{};
    inputParameters.device = inputDevice;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = inputInfo->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParameters{};
    outputParameters.device = outputDevice;
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = outputInfo->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    const std::array<double, 4> candidateRates{
        48000.0,
        44100.0,
        inputInfo->defaultSampleRate,
        outputInfo->defaultSampleRate
    };

    PaError lastError = paInvalidSampleRate;
    for (std::size_t i = 0; i < candidateRates.size(); ++i) {
        const double sampleRate = candidateRates[i];
        if (sampleRate <= 0.0) continue;

        bool duplicate = false;
        for (std::size_t j = 0; j < i; ++j) {
            if (candidateRates[j] == sampleRate) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;

        const PaError formatResult = Pa_IsFormatSupported(
            &inputParameters, &outputParameters, sampleRate);
        if (formatResult != paFormatIsSupported) {
            lastError = formatResult;
            continue;
        }

        lastError = Pa_OpenStream(
            &stream_,
            &inputParameters,
            &outputParameters,
            sampleRate,
            256,
            paNoFlag,
            &AudioEngine::audioCallback,
            this);

        if (lastError == paNoError) break;
        stream_ = nullptr;
    }

    if (lastError != paNoError || stream_ == nullptr) {
        if (errorMessage) {
            *errorMessage = QString("Could not open microphone and speaker together: %1")
                                .arg(Pa_GetErrorText(lastError));
        }
        return false;
    }

    const PaError startResult = Pa_StartStream(stream_);
    if (startResult != paNoError) {
        if (errorMessage) {
            *errorMessage = QString("Could not start audio stream: %1").arg(Pa_GetErrorText(startResult));
        }
        Pa_CloseStream(stream_);
        stream_ = nullptr;
        return false;
    }

    return true;
}

void AudioEngine::stop()
{
    if (stream_ == nullptr) return;

    if (Pa_IsStreamActive(stream_) == 1) {
        Pa_StopStream(stream_);
    } else {
        Pa_AbortStream(stream_);
    }
    Pa_CloseStream(stream_);
    stream_ = nullptr;
    inputLevel_.store(0.0F, std::memory_order_relaxed);
}

bool AudioEngine::isRunning() const
{
    return stream_ != nullptr && Pa_IsStreamActive(stream_) == 1;
}

float AudioEngine::inputLevel() const
{
    return inputLevel_.load(std::memory_order_relaxed);
}

int AudioEngine::audioCallback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags,
    void *userData)
{
    auto *engine = static_cast<AudioEngine *>(userData);
    const auto *inputSamples = static_cast<const float *>(input);
    auto *outputSamples = static_cast<float *>(output);

    if (outputSamples == nullptr) return paContinue;

    double sumSquares = 0.0;
    for (unsigned long i = 0; i < frameCount; ++i) {
        const float sample = inputSamples != nullptr ? inputSamples[i] : 0.0F;
        outputSamples[i] = sample;
        sumSquares += static_cast<double>(sample) * static_cast<double>(sample);
    }

    const float rms = frameCount > 0
        ? static_cast<float>(std::sqrt(sumSquares / static_cast<double>(frameCount)))
        : 0.0F;
    engine->inputLevel_.store(std::clamp(rms, 0.0F, 1.0F), std::memory_order_relaxed);
    return paContinue;
}
