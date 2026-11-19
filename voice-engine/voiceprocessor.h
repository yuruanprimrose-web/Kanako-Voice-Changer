#pragma once

#include <signalsmith-stretch/signalsmith-stretch.h>
#include <atomic>
#include <vector>

class VoiceProcessor final
{
public:
    void prepare(double sampleRate);
    void setVoiceTone(float pitchSemitones, float formantSemitones);
    void process(const float *input, float *output, unsigned long frameCount, float inputGain = 1.0F);

private:
    signalsmith::stretch::SignalsmithStretch<float> stretch_;
    std::vector<float> gainedInput_;
    std::atomic<float> requestedPitch_{0.0F};
    std::atomic<float> requestedFormant_{0.0F};
    float activePitch_ = 0.0F;
    float activeFormant_ = 0.0F;
    bool prepared_ = false;
};
