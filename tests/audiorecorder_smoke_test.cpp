#include "audiorecorder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {
std::uint32_t readU32(const QByteArray &bytes, int offset)
{
    const auto *p = reinterpret_cast<const unsigned char *>(bytes.constData() + offset);
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}
}

int main()
{
    const QString path = QDir::tempPath() + "/kanako-recorder-smoke-test.wav";
    QFile::remove(path);

    AudioRecorder recorder;
    QString error;
    if (!recorder.start(path, 48000, &error)) {
        std::cerr << "Recorder start failed: " << error.toStdString() << '\n';
        return 1;
    }

    constexpr std::size_t sampleCount = 48000;
    std::vector<float> samples(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i)
        samples[i] = 0.25F * std::sin(2.0 * 3.14159265358979323846 * 440.0 * static_cast<double>(i) / 48000.0);

    recorder.push(samples.data(), samples.size());
    const auto accepted = recorder.acceptedSamples();
    const auto peak = recorder.peakLevel();
    const auto dropped = recorder.droppedSamples();
    recorder.stop();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "WAV output could not be opened.\n";
        return 2;
    }
    const QByteArray wav = file.readAll();
    file.close();
    QFile::remove(path);

    if (dropped != 0 || accepted != sampleCount || peak < 0.24F) {
        std::cerr << "Recorder input diagnostics failed.\n";
        return 3;
    }
    if (wav.size() != 44 + static_cast<qsizetype>(sampleCount * 2) ||
        wav.mid(0, 4) != "RIFF" || wav.mid(8, 4) != "WAVE" ||
        wav.mid(36, 4) != "data" ||
        readU32(wav, 40) != sampleCount * 2) {
        std::cerr << "WAV header or payload size is invalid.\n";
        return 4;
    }

    bool hasNonZeroPcm = false;
    for (qsizetype i = 44; i + 1 < wav.size(); i += 2) {
        if (wav.at(i) != 0 || wav.at(i + 1) != 0) {
            hasNonZeroPcm = true;
            break;
        }
    }
    if (!hasNonZeroPcm) {
        std::cerr << "WAV payload contains only silence.\n";
        return 5;
    }

    std::cout << "PASS: WAV header, payload, sample count, and non-zero PCM verified.\n";
    return 0;
}
