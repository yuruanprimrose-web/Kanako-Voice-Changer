#include "voiceprocessor.h"

#include <algorithm>

void VoiceProcessor::prepare(double sampleRate)
{
    const float rate = static_cast<float>(sampleRate);
    stretch_.presetCheaper(1, rate, true);
    stretch_.setFormantBase(0.0F);
    activePreset_ = static_cast<Preset>(requestedPreset_.load(std::memory_order_relaxed));
    applyPreset(activePreset_);
    prepared_ = true;
}

void VoiceProcessor::setPreset(Preset preset)
{
    requestedPreset_.store(static_cast<int>(preset), std::memory_order_relaxed);
}

void VoiceProcessor::applyPreset(Preset preset)
{
    switch (preset) {
    case Preset::Female:
        stretch_.setTransposeSemitones(4.0F);
        stretch_.setFormantSemitones(2.0F, true);
        break;
    case Preset::Male:
        stretch_.setTransposeSemitones(-3.0F);
        stretch_.setFormantSemitones(-1.5F, true);
        break;
    case Preset::Neutral:
    default:
        stretch_.setTransposeSemitones(0.0F);
        stretch_.setFormantSemitones(0.0F);
        break;
    }
}

void VoiceProcessor::process(const float *input, float *output, unsigned long frameCount)
{
    if (output == nullptr || frameCount == 0) return;
    if (input == nullptr || !prepared_) {
        std::fill(output, output + frameCount, 0.0F);
        return;
    }

    const auto requested = static_cast<Preset>(requestedPreset_.load(std::memory_order_relaxed));
    if (requested != activePreset_) {
        applyPreset(requested);
        activePreset_ = requested;
    }

    float *inputChannels[1] = {const_cast<float *>(input)};
    float *outputChannels[1] = {output};
    stretch_.process(inputChannels, static_cast<int>(frameCount),
                     outputChannels, static_cast<int>(frameCount));
}
