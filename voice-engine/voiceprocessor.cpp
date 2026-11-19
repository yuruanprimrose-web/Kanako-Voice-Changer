#include "voiceprocessor.h"

#include <algorithm>

void VoiceProcessor::prepare(double sampleRate)
{
    const float rate = static_cast<float>(sampleRate);
    gainedInput_.resize(8192);
    stretch_.presetCheaper(1, rate, true);
    stretch_.setFormantBase(0.0F);
    activePitch_ = requestedPitch_.load(std::memory_order_relaxed);
    activeFormant_ = requestedFormant_.load(std::memory_order_relaxed);
    stretch_.setTransposeSemitones(activePitch_);
    stretch_.setFormantSemitones(activeFormant_, true);
    prepared_ = true;
}

void VoiceProcessor::setVoiceTone(float pitchSemitones, float formantSemitones)
{
    requestedPitch_.store(std::clamp(pitchSemitones, -12.0F, 12.0F), std::memory_order_relaxed);
    requestedFormant_.store(std::clamp(formantSemitones, -6.0F, 6.0F), std::memory_order_relaxed);
}

void VoiceProcessor::process(const float *input, float *output, unsigned long frameCount, float inputGain)
{
    if (output == nullptr || frameCount == 0) return;
    if (input == nullptr || !prepared_) {
        std::fill(output, output + frameCount, 0.0F);
        return;
    }

    const float requestedPitch = requestedPitch_.load(std::memory_order_relaxed);
    const float requestedFormant = requestedFormant_.load(std::memory_order_relaxed);
    if (requestedPitch != activePitch_) {
        stretch_.setTransposeSemitones(requestedPitch);
        activePitch_ = requestedPitch;
    }
    if (requestedFormant != activeFormant_) {
        stretch_.setFormantSemitones(requestedFormant, true);
        activeFormant_ = requestedFormant;
    }

    if (frameCount > gainedInput_.size()) gainedInput_.resize(frameCount);
    const float safeInputGain = std::clamp(inputGain, 0.0F, 4.0F);
    for (unsigned long i = 0; i < frameCount; ++i)
        gainedInput_[i] = input[i] * safeInputGain;

    float *inputChannels[1] = {gainedInput_.data()};
    float *outputChannels[1] = {output};
    stretch_.process(inputChannels, static_cast<int>(frameCount),
                     outputChannels, static_cast<int>(frameCount));
}
