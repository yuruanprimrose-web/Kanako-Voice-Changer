#pragma once

#include "audioengine.h"
#include <QMainWindow>

class QComboBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QSlider;
class QTimer;

class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
private:
    void initializeAudioDevices();
    void populateDeviceLists();
    void restoreSettings();
    void saveSettings();
    void startAudio();
    void stopAudio();
    void updateInputMeter();
    void setAudioStatus(const QString &message, bool isError = false);

    AudioEngine audioEngine_;
    QComboBox *inputDeviceCombo_ = nullptr;
    QComboBox *outputDeviceCombo_ = nullptr;
    QComboBox *presetCombo_ = nullptr;
    QSlider *pitchSlider_ = nullptr;
    QSlider *formantSlider_ = nullptr;
    QLabel *pitchValue_ = nullptr;
    QLabel *formantValue_ = nullptr;
    QSlider *microphoneGainSlider_ = nullptr;
    QSlider *outputGainSlider_ = nullptr;
    QLabel *microphoneGainValue_ = nullptr;
    QLabel *outputGainValue_ = nullptr;
    QLabel *audioStatusLabel_ = nullptr;
    QProgressBar *inputMeter_ = nullptr;
    QPushButton *refreshDevicesButton_ = nullptr;
    QPushButton *startButton_ = nullptr;
    QPushButton *stopButton_ = nullptr;
    QTimer *meterTimer_ = nullptr;
    bool portAudioInitialized_ = false;
};
