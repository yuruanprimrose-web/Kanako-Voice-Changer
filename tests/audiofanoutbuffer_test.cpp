#include "audiofanoutbuffer.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>

int main()
{
    AudioFanoutBuffer buffer;
    std::array<float, 8> input{0.1F, 0.2F, 0.3F, 0.4F, 0.5F, 0.6F, 0.7F, 0.8F};
    std::array<float, 10> output{};

    if (buffer.write(input.data(), input.size()) != input.size()) return 1;
    if (buffer.queuedSamples() != input.size()) return 2;
    if (buffer.read(output.data(), output.size()) != input.size()) return 3;
    for (std::size_t i = 0; i < input.size(); ++i)
        if (std::abs(output[i] - input[i]) > 0.00001F) return 4;
    if (output[8] != 0.0F || output[9] != 0.0F) return 5;
    if (buffer.underflowSamples() != 2) return 6;

    std::array<float, AudioFanoutBuffer::kCapacity + 4> oversized{};
    oversized.fill(0.25F);
    if (buffer.write(oversized.data(), oversized.size()) != AudioFanoutBuffer::kCapacity) return 7;
    if (buffer.droppedSamples() != 4) return 8;

    std::array<float, 16> prefix{};
    if (buffer.read(prefix.data(), prefix.size()) != prefix.size()) return 9;
    for (float sample : prefix)
        if (std::abs(sample - 0.25F) > 0.00001F) return 10;

    buffer.reset();
    if (buffer.queuedSamples() != 0 || buffer.droppedSamples() != 0 ||
        buffer.underflowSamples() != 0) return 11;

    std::cout << "Audio fan-out buffer tests passed.\n";
    return 0;
}
