#pragma once

#include <QString>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <thread>
#include <vector>

class AudioRecorder final
{
public:
    AudioRecorder();
    ~AudioRecorder();
    AudioRecorder(const AudioRecorder &) = delete;
    AudioRecorder &operator=(const AudioRecorder &) = delete;

    bool start(const QString &filePath, unsigned int sampleRate, QString *errorMessage);
    void push(const float *samples, std::size_t count) noexcept;
    void pause() noexcept;
    void resume() noexcept;
    void stop() noexcept;
    bool isRecording() const noexcept;
    bool isPaused() const noexcept;
    std::uint64_t droppedSamples() const noexcept;

private:
    void writerLoop() noexcept;
    void writeWaveHeader(std::uint32_t dataBytes);
    static constexpr std::size_t kCapacity = 1U << 20U;

    std::vector<float> ring_;
    std::atomic<std::uint64_t> writeIndex_{0};
    std::atomic<std::uint64_t> readIndex_{0};
    std::atomic<bool> active_{false};
    std::atomic<bool> paused_{false};
    std::atomic<bool> stopping_{false};
    std::atomic<std::uint64_t> dropped_{0};
    std::thread writer_;
    std::ofstream file_;
    unsigned int sampleRate_ = 48000;
    std::uint64_t dataBytes_ = 0;
};
