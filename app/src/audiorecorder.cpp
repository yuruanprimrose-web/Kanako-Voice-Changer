#include "audiorecorder.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>

namespace {
void writeU16(std::ostream &out, std::uint16_t value)
{
    const char bytes[2] = {static_cast<char>(value & 0xffU), static_cast<char>((value >> 8U) & 0xffU)};
    out.write(bytes, 2);
}
void writeU32(std::ostream &out, std::uint32_t value)
{
    const char bytes[4] = {static_cast<char>(value & 0xffU), static_cast<char>((value >> 8U) & 0xffU),
                           static_cast<char>((value >> 16U) & 0xffU), static_cast<char>((value >> 24U) & 0xffU)};
    out.write(bytes, 4);
}
}

AudioRecorder::AudioRecorder() : ring_(kCapacity, 0.0F) {}
AudioRecorder::~AudioRecorder() { stop(); }

bool AudioRecorder::start(const QString &filePath, unsigned int sampleRate, QString *errorMessage)
{
    stop();
    file_.open(filePath.toStdString(), std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file_.is_open()) {
        if (errorMessage) *errorMessage = "Could not create the recording file. Check the folder permissions.";
        return false;
    }
    sampleRate_ = sampleRate;
    dataBytes_ = 0;
    dropped_.store(0, std::memory_order_relaxed);
    writeIndex_.store(0, std::memory_order_relaxed);
    readIndex_.store(0, std::memory_order_relaxed);
    writeWaveHeader(0);
    file_.seekp(44);
    paused_.store(false, std::memory_order_relaxed);
    stopping_.store(false, std::memory_order_relaxed);
    active_.store(true, std::memory_order_release);
    writer_ = std::thread(&AudioRecorder::writerLoop, this);
    return true;
}

void AudioRecorder::push(const float *samples, std::size_t count) noexcept
{
    if (!active_.load(std::memory_order_acquire) || paused_.load(std::memory_order_relaxed) || samples == nullptr) return;
    auto write = writeIndex_.load(std::memory_order_relaxed);
    const auto read = readIndex_.load(std::memory_order_acquire);
    const auto available = kCapacity - static_cast<std::size_t>(write - read);
    const auto accepted = std::min(count, available);
    for (std::size_t i = 0; i < accepted; ++i) ring_[static_cast<std::size_t>((write + i) % kCapacity)] = samples[i];
    writeIndex_.store(write + accepted, std::memory_order_release);
    if (accepted < count) dropped_.fetch_add(count - accepted, std::memory_order_relaxed);
}

void AudioRecorder::pause() noexcept { paused_.store(true, std::memory_order_release); }
void AudioRecorder::resume() noexcept { if (active_.load(std::memory_order_acquire)) paused_.store(false, std::memory_order_release); }
bool AudioRecorder::isRecording() const noexcept { return active_.load(std::memory_order_acquire); }
bool AudioRecorder::isPaused() const noexcept { return paused_.load(std::memory_order_acquire); }
std::uint64_t AudioRecorder::droppedSamples() const noexcept { return dropped_.load(std::memory_order_relaxed); }

void AudioRecorder::stop() noexcept
{
    if (!active_.exchange(false, std::memory_order_acq_rel) && !writer_.joinable()) return;
    stopping_.store(true, std::memory_order_release);
    if (writer_.joinable()) writer_.join();
    if (file_.is_open()) {
        writeWaveHeader(static_cast<std::uint32_t>(std::min<std::uint64_t>(dataBytes_, std::numeric_limits<std::uint32_t>::max() - 36ULL)));
        file_.flush();
        file_.close();
    }
}

void AudioRecorder::writeWaveHeader(std::uint32_t dataBytes)
{
    file_.seekp(0);
    file_.write("RIFF", 4); writeU32(file_, 36U + dataBytes); file_.write("WAVE", 4);
    file_.write("fmt ", 4); writeU32(file_, 16); writeU16(file_, 1); writeU16(file_, 1);
    writeU32(file_, sampleRate_); writeU32(file_, sampleRate_ * 2U); writeU16(file_, 2); writeU16(file_, 16);
    file_.write("data", 4); writeU32(file_, dataBytes);
}

void AudioRecorder::writerLoop() noexcept
{
    constexpr std::size_t chunkSize = 4096;
    float chunk[chunkSize];
    while (true) {
        auto read = readIndex_.load(std::memory_order_relaxed);
        const auto write = writeIndex_.load(std::memory_order_acquire);
        const auto pending = static_cast<std::size_t>(write - read);
        if (pending == 0) {
            if (stopping_.load(std::memory_order_acquire)) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(4));
            continue;
        }
        const auto count = std::min(pending, chunkSize);
        for (std::size_t i = 0; i < count; ++i) chunk[i] = ring_[static_cast<std::size_t>((read + i) % kCapacity)];
        readIndex_.store(read + count, std::memory_order_release);
        for (std::size_t i = 0; i < count; ++i) {
            const float sample = std::clamp(chunk[i], -1.0F, 1.0F);
            const auto pcm = static_cast<std::int16_t>(std::lrint(sample * (sample < 0.0F ? 32768.0F : 32767.0F)));
            writeU16(file_, static_cast<std::uint16_t>(pcm));
        }
        dataBytes_ += count * 2U;
        if (!file_) { stopping_.store(true, std::memory_order_release); break; }
    }
}
