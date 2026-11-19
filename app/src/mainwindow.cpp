#include "mainwindow.h"
#include <portaudio.h>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Kanako Voice Changer");
    setMinimumSize(640, 460);
    resize(760, 540);
    auto *central = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(28, 24, 28, 24);
    rootLayout->setSpacing(18);

    auto *heading = new QLabel("KANAKO", central);
    QFont headingFont = heading->font();
    headingFont.setPointSize(22);
    headingFont.setBold(true);
    heading->setFont(headingFont);
    auto *subtitle = new QLabel("Voice Changer", central);

    auto *deviceGroup = new QGroupBox("Audio devices", central);
    auto *deviceLayout = new QFormLayout(deviceGroup);
    inputDeviceCombo_ = new QComboBox(deviceGroup);
    inputDeviceCombo_->setObjectName("inputDeviceCombo");
    outputDeviceCombo_ = new QComboBox(deviceGroup);
    outputDeviceCombo_->setObjectName("outputDeviceCombo");
    deviceLayout->addRow("Microphone", inputDeviceCombo_);
    deviceLayout->addRow("Speaker / Output", outputDeviceCombo_);

    auto *presetGroup = new QGroupBox("Voice preset", central);
    auto *presetLayout = new QFormLayout(presetGroup);
    auto *presetCombo = new QComboBox(presetGroup);
    presetCombo->setObjectName("voicePresetCombo");
    presetCombo->addItem("Neutral / Bypass", "neutral");
    presetCombo->addItem("Female-style", "female");
    presetCombo->addItem("Male-style", "male");
    presetCombo->setEnabled(false);
    presetLayout->addRow("Select preset", presetCombo);

    inputMeterLabel_ = new QLabel("Input level: waiting for audio engine", central);
    audioStatusLabel_ = new QLabel("Initializing audio devices...", central);
    audioStatusLabel_->setWordWrap(true);
    startButton_ = new QPushButton("Start voice changer", central);
    startButton_->setObjectName("startButton");
    startButton_->setEnabled(false);
    startButton_->setToolTip("Live audio processing will be enabled after the audio pipeline is implemented.");
    auto *note = new QLabel(
        "Development build — device discovery is available. Live audio processing and voice presets are not connected yet.",
        central);
    note->setWordWrap(true);

    rootLayout->addWidget(heading);
    rootLayout->addWidget(subtitle);
    rootLayout->addWidget(deviceGroup);
    rootLayout->addWidget(presetGroup);
    rootLayout->addWidget(inputMeterLabel_);
    rootLayout->addWidget(audioStatusLabel_);
    rootLayout->addWidget(startButton_);
    rootLayout->addStretch();
    rootLayout->addWidget(note);
    setCentralWidget(central);
    initializeAudioDevices();
}

MainWindow::~MainWindow()
{
    if (portAudioInitialized_) {
        Pa_Terminate();
    }
}

void MainWindow::initializeAudioDevices()
{
    const PaError result = Pa_Initialize();
    if (result != paNoError) {
        setAudioStatus(QString("PortAudio initialization failed: %1").arg(Pa_GetErrorText(result)), true);
        return;
    }
    portAudioInitialized_ = true;
    populateDeviceLists();
    const PaDeviceIndex deviceCount = Pa_GetDeviceCount();
    if (deviceCount < 0) {
        setAudioStatus(QString("Could not enumerate audio devices: %1").arg(Pa_GetErrorText(deviceCount)), true);
        return;
    }
    setAudioStatus(QString("Audio library initialized. Found %1 device(s). Live processing is not enabled in this build.")
                       .arg(deviceCount));
}

void MainWindow::populateDeviceLists()
{
    inputDeviceCombo_->clear();
    outputDeviceCombo_->clear();
    const PaDeviceIndex deviceCount = Pa_GetDeviceCount();
    if (deviceCount < 0) {
        inputDeviceCombo_->addItem("Audio devices unavailable");
        outputDeviceCombo_->addItem("Audio devices unavailable");
        return;
    }
    for (PaDeviceIndex index = 0; index < deviceCount; ++index) {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(index);
        if (info == nullptr) continue;
        const QString deviceName = QString::fromUtf8(info->name);
        if (info->maxInputChannels > 0) inputDeviceCombo_->addItem(deviceName, index);
        if (info->maxOutputChannels > 0) outputDeviceCombo_->addItem(deviceName, index);
    }
    if (inputDeviceCombo_->count() == 0) inputDeviceCombo_->addItem("No microphone detected");
    if (outputDeviceCombo_->count() == 0) outputDeviceCombo_->addItem("No output device detected");
}

void MainWindow::setAudioStatus(const QString &message, bool isError)
{
    audioStatusLabel_->setText(message);
    audioStatusLabel_->setStyleSheet(isError ? "color: #b42318;" : "color: palette(text);");
}
