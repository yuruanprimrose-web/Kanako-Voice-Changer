#include "audioengine.h"

#include <algorithm>
#include <array>
#include <cmath>

AudioEngine::~AudioEngine()
{
    stop();
}

bool AudioEngine::start(PaDeviceIndex inputDevice, PaDeviceIndex outputDevice,
                        PaDeviceIndex virtualMicDevice, QString *errorMessage)
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
        48000.0, 44100.0, inputInfo->defaultSampleRate, outputInfo->defaultSampleRate
    };

    PaError lastError = paInvalidSampleRate;
    double openedSampleRate = 0.0;
    for (std::size_t i = 0; i < candidateRates.size(); ++i) {
        const double sampleRate = candidateRates[i];
        if (sampleRate <= 0.0) continue;
        bool duplicate = false;
        for (std::size_t j = 0; j < i; ++j) {
            if (candidateRates[j] == sampleRate) { duplicate = true; break; }
        }
        if (duplicate) continue;

        const PaError formatResult = Pa_IsFormatSupported(&inputParameters, &outputParameters, sampleRate);
        if (formatResult != paFormatIsSupported) { lastError = formatResult; continue; }

        lastError = Pa_OpenStream(&stream_, &inputParameters, &outputParameters, sampleRate, 256,
                                  paNoFlag, &AudioEngine::audioCallback, this);
        if (lastError == paNoError) { openedSampleRate = sampleRate; break; }
        stream_ = nullptr;
    }

    if (lastError != paNoError || stream_ == nullptr) {
        if (errorMessage) *errorMessage = QString("Could not open microphone and speaker together: %1").arg(Pa_GetErrorText(lastError));
        return false;
    }

    sampleRate_ = openedSampleRate;
    processor_.prepare(openedSampleRate);

    if (virtualMicDevice != paNoDevice) {
        const PaDeviceInfo *virtualInfo = Pa_GetDeviceInfo(virtualMicDevice);
        if (virtualInfo == nullptr || virtualInfo->maxOutputChannels < 1) {
            if (errorMessage) *errorMessage = "The selected virtual microphone playback endpoint is unavailable. Refresh audio devices and try again.";
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            return false;
        }

        PaStreamParameters virtualOutput{};
        virtualOutput.device = virtualMicDevice;
        virtualOutput.channelCount = 1;
        virtualOutput.sampleFormat = paFloat32;
        virtualOutput.suggestedLatency = virtualInfo->defaultLowOutputLatency;
        virtualOutput.hostApiSpecificStreamInfo = nullptr;

        const PaError virtualFormat = Pa_IsFormatSupported(nullptr, &virtualOutput, sampleRate_);
        if (virtualFormat != paFormatIsSupported) {
            if (errorMessage) *errorMessage = QString("The virtual microphone endpoint does not support %1 Hz audio: %2")
                .arg(static_cast<int>(sampleRate_)).arg(Pa_GetErrorText(virtualFormat));
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            return false;
        }

        virtualMicBuffer_.reset();
        const PaError openVirtual = Pa_OpenStream(&virtualMicStream_, nullptr, &virtualOutput, sampleRate_, 256,
                                                   paNoFlag, &AudioEngine::virtualMicCallback, this);
        if (openVirtual != paNoError || virtualMicStream_ == nullptr) {
            if (errorMessage) *errorMessage = QString("Could not open the virtual microphone output: %1").arg(Pa_GetErrorText(openVirtual));
            virtualMicStream_ = nullptr;
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            return false;
        }
    }

    const PaError startResult = Pa_StartStream(stream_);
    if (startResult != paNoError) {
        if (errorMessage) *errorMessage = QString("Could not start audio stream: %1").arg(Pa_GetErrorText(startResult));
        if (virtualMicStream_ != nullptr) {
            Pa_CloseStream(virtualMicStream_);
            virtualMicStream_ = nullptr;
        }
        Pa_CloseStream(stream_);
        stream_ = nullptr;
        return false;
    }

    if (virtualMicStream_ != nullptr) {
        const PaError virtualStart = Pa_StartStream(virtualMicStream_);
        if (virtualStart != paNoError) {
            if (errorMessage) *errorMessage = QString("Could not start virtual microphone output: %1").arg(Pa_GetErrorText(virtualStart));
            Pa_StopStream(stream_);
            Pa_CloseStream(virtualMicStream_);
            virtualMicStream_ = nullptr;
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            return false;
        }
    }
    return true;
}
void AudioEngine::stop()
{
    if (virtualMicStream_ != nullptr) {
        const PaError active = Pa_IsStreamActive(virtualMicStream_);
        if (active == 1) Pa_StopStream(virtualMicStream_);
        else Pa_AbortStream(virtualMicStream_);
        Pa_CloseStream(virtualMicStream_);
        virtualMicStream_ = nullptr;
    }
    if (stream_ != nullptr) {
        const PaError active = Pa_IsStreamActive(stream_);
        if (active == 1) Pa_StopStream(stream_);
        else Pa_AbortStream(stream_);
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }
    recorder_.stop();
    virtualMicBuffer_.reset();
    inputLevel_.store(0.0F, std::memory_order_relaxed);
}
bool AudioEngine::isRunning() const
{
    return stream_ != nullptr && Pa_IsStreamActive(stream_) == 1;
}

void AudioEngine::setVoiceTone(float pitchSemitones, float formantSemitones)
{
    processor_.setVoiceTone(pitchSemitones, formantSemitones);
}

void AudioEngine::setMicrophoneGain(float gain)
{
    microphoneGain_.store(std::clamp(gain, 0.0F, 4.0F), std::memory_order_relaxed);
}

void AudioEngine::setOutputGain(float gain)
{
    outputGain_.store(std::clamp(gain, 0.0F, 3.0F), std::memory_order_relaxed);
}

float AudioEngine::inputLevel() const
{
    return inputLevel_.load(std::memory_order_relaxed);
}

bool AudioEngine::startRecording(const QString &filePath, QString *errorMessage)
{
    if (!isRunning()) {
        if (errorMessage) *errorMessage = "Start the voice changer before recording.";
        return false;
    }
    return recorder_.start(filePath, static_cast<unsigned int>(sampleRate_), errorMessage);
}

void AudioEngine::pauseRecording() { recorder_.pause(); }
void AudioEngine::resumeRecording() { recorder_.resume(); }
void AudioEngine::stopRecording() { recorder_.stop(); }
bool AudioEngine::isRecording() const { return recorder_.isRecording(); }
bool AudioEngine::isRecordingPaused() const { return recorder_.isPaused(); }
std::uint64_t AudioEngine::droppedRecordingSamples() const { return recorder_.droppedSamples(); }
std::uint64_t AudioEngine::acceptedRecordingSamples() const { return recorder_.acceptedSamples(); }
float AudioEngine::recordingPeakLevel() const { return recorder_.peakLevel(); }
std::uint64_t AudioEngine::virtualMicDroppedSamples() const { return virtualMicBuffer_.droppedSamples(); }
std::uint64_t AudioEngine::virtualMicUnderflowSamples() const { return virtualMicBuffer_.underflowSamples(); }
bool AudioEngine::isVirtualMicActive() const { return virtualMicStream_ != nullptr; }

int AudioEngine::virtualMicCallback(const void *, void *output, unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags, void *userData)
{
    auto *engine = static_cast<AudioEngine *>(userData);
    auto *outputSamples = static_cast<float *>(output);
    if (outputSamples == nullptr) return paContinue;
    engine->virtualMicBuffer_.read(outputSamples, static_cast<std::size_t>(frameCount));
    return paContinue;
}

int AudioEngine::audioCallback(const void *input, void *output, unsigned long frameCount,
                               const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags, void *userData)
{
    auto *engine = static_cast<AudioEngine *>(userData);
    const auto *inputSamples = static_cast<const float *>(input);
    auto *outputSamples = static_cast<float *>(output);
    if (outputSamples == nullptr) return paContinue;

    const float micGain = engine->microphoneGain_.load(std::memory_order_relaxed);
    const float outputGain = engine->outputGain_.load(std::memory_order_relaxed);
    engine->processor_.process(inputSamples, outputSamples, frameCount, micGain);

    constexpr float limitStart = 0.90F;
    constexpr float limitRange = 0.10F;
    for (unsigned long i = 0; i < frameCount; ++i) {
        const float amplified = outputSamples[i] * outputGain;
        const float magnitude = std::abs(amplified);
        if (magnitude > limitStart) {
            const float limited = limitStart + limitRange * std::tanh((magnitude - limitStart) / limitRange);
            outputSamples[i] = std::copysign(limited, amplified);
        } else outputSamples[i] = amplified;
    }

    double sumSquares = 0.0;
    for (unsigned long i = 0; i < frameCount; ++i) {
        const float sample = inputSamples != nullptr ? inputSamples[i] : 0.0F;
        sumSquares += static_cast<double>(sample) * static_cast<double>(sample);
    }
    const float rms = frameCount > 0 ? static_cast<float>(std::sqrt(sumSquares / static_cast<double>(frameCount))) : 0.0F;
    engine->recorder_.push(outputSamples, static_cast<std::size_t>(frameCount));
    if (engine->virtualMicStream_ != nullptr)
        engine->virtualMicBuffer_.write(outputSamples, static_cast<std::size_t>(frameCount));
    engine->inputLevel_.store(std::clamp(rms, 0.0F, 1.0F), std::memory_order_relaxed);
    return paContinue;
}
