#pragma once

#include <signalsmith-stretch/signalsmith-stretch.h>
#include <atomic>
#include <cstddef>
#include <vector>

class VoiceProcessor final
{
public:
    enum class Preset : int {
        Neutral = 0,
        Female = 1,
        Male = 2
    };

    void prepare(double sampleRate);
    void setPreset(Preset preset);
    void process(const float *input, float *output, unsigned long frameCount, float inputGain = 1.0F);

private:
    void applyPreset(Preset preset);

    signalsmith::stretch::SignalsmithStretch<float> stretch_;
    std::vector<float> gainedInput_;
    std::atomic<int> requestedPreset_{static_cast<int>(Preset::Neutral)};
    Preset activePreset_ = Preset::Neutral;
    bool prepared_ = false;
};
