#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>

// Single-producer/single-consumer float queue for decoupling independent
// PortAudio output callbacks. Reset only while both streams are stopped.
class AudioFanoutBuffer final
{
public:
    static constexpr std::size_t kCapacity = 1U << 16U;

    AudioFanoutBuffer() : samples_(kCapacity, 0.0F) {}

    AudioFanoutBuffer(const AudioFanoutBuffer &) = delete;
    AudioFanoutBuffer &operator=(const AudioFanoutBuffer &) = delete;

    void reset() noexcept
    {
        readIndex_.store(0, std::memory_order_relaxed);
        writeIndex_.store(0, std::memory_order_relaxed);
        droppedSamples_.store(0, std::memory_order_relaxed);
        underflowSamples_.store(0, std::memory_order_relaxed);
    }

    std::size_t write(const float *input, std::size_t count) noexcept
    {
        if (input == nullptr || count == 0) return 0;
        const auto write = writeIndex_.load(std::memory_order_relaxed);
        const auto read = readIndex_.load(std::memory_order_acquire);
        const auto used = static_cast<std::size_t>(write - read);
        const auto available = used < kCapacity ? kCapacity - used : 0;
        const auto accepted = std::min(count, available);
        for (std::size_t i = 0; i < accepted; ++i)
            samples_[static_cast<std::size_t>((write + i) % kCapacity)] = input[i];
        writeIndex_.store(write + accepted, std::memory_order_release);
        if (accepted < count)
            droppedSamples_.fetch_add(count - accepted, std::memory_order_relaxed);
        return accepted;
    }

    std::size_t read(float *output, std::size_t count) noexcept
    {
        if (output == nullptr || count == 0) return 0;
        const auto read = readIndex_.load(std::memory_order_relaxed);
        const auto write = writeIndex_.load(std::memory_order_acquire);
        const auto available = static_cast<std::size_t>(write - read);
        const auto copied = std::min(count, available);
        for (std::size_t i = 0; i < copied; ++i)
            output[i] = samples_[static_cast<std::size_t>((read + i) % kCapacity)];
        std::fill(output + copied, output + count, 0.0F);
        readIndex_.store(read + copied, std::memory_order_release);
        if (copied < count)
            underflowSamples_.fetch_add(count - copied, std::memory_order_relaxed);
        return copied;
    }

    std::uint64_t droppedSamples() const noexcept
    {
        return droppedSamples_.load(std::memory_order_relaxed);
    }

    std::uint64_t underflowSamples() const noexcept
    {
        return underflowSamples_.load(std::memory_order_relaxed);
    }

    std::size_t queuedSamples() const noexcept
    {
        const auto write = writeIndex_.load(std::memory_order_acquire);
        const auto read = readIndex_.load(std::memory_order_acquire);
        return static_cast<std::size_t>(write - read);
    }

private:
    std::vector<float> samples_;
    alignas(64) std::atomic<std::uint64_t> writeIndex_{0};
    alignas(64) std::atomic<std::uint64_t> readIndex_{0};
    std::atomic<std::uint64_t> droppedSamples_{0};
    std::atomic<std::uint64_t> underflowSamples_{0};
};
