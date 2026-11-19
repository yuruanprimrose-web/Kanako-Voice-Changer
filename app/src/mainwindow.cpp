#include "mainwindow.h"
#include <portaudio.h>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Kanako Voice Changer");
    setMinimumSize(640, 480);
    resize(760, 560);
    auto *central = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(28, 24, 28, 24);
    rootLayout->setSpacing(16);

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
    presetCombo_ = new QComboBox(presetGroup);
    presetCombo_->setObjectName("voicePresetCombo");
    presetCombo_->addItem("Neutral / Bypass", "neutral");
    presetCombo_->addItem("Female-style", "female");
    presetCombo_->addItem("Male-style", "male");
    presetLayout->addRow("Select preset", presetCombo_);

    auto *meterLabel = new QLabel("Microphone input level", central);
    inputMeter_ = new QProgressBar(central);
    inputMeter_->setObjectName("inputLevelMeter");
    inputMeter_->setRange(0, 100);
    inputMeter_->setValue(0);
    inputMeter_->setTextVisible(false);

    audioStatusLabel_ = new QLabel("Initializing audio devices...", central);
    audioStatusLabel_->setWordWrap(true);
    startButton_ = new QPushButton("Start voice changer", central);
    startButton_->setObjectName("startButton");
    stopButton_ = new QPushButton("Stop", central);
    stopButton_->setObjectName("stopButton");
    stopButton_->setEnabled(false);

    auto *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(startButton_);
    buttonLayout->addWidget(stopButton_);
    auto *note = new QLabel(
        "Development build — non-AI pitch and formant presets are connected as an initial prototype. Use headphones to reduce acoustic feedback.",
        central);
    note->setWordWrap(true);

    rootLayout->addWidget(heading);
    rootLayout->addWidget(subtitle);
    rootLayout->addWidget(deviceGroup);
    rootLayout->addWidget(presetGroup);
    rootLayout->addWidget(meterLabel);
    rootLayout->addWidget(inputMeter_);
    rootLayout->addWidget(audioStatusLabel_);
    rootLayout->addLayout(buttonLayout);
    rootLayout->addStretch();
    rootLayout->addWidget(note);
    setCentralWidget(central);

    connect(presetCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        audioEngine_.setPreset(static_cast<VoiceProcessor::Preset>(index));
    });
    connect(startButton_, &QPushButton::clicked, this, &MainWindow::startAudio);
    connect(stopButton_, &QPushButton::clicked, this, &MainWindow::stopAudio);
    meterTimer_ = new QTimer(this);
    meterTimer_->setInterval(33);
    connect(meterTimer_, &QTimer::timeout, this, &MainWindow::updateInputMeter);
    initializeAudioDevices();
}

MainWindow::~MainWindow()
{
    meterTimer_->stop();
    audioEngine_.stop();
    if (portAudioInitialized_) Pa_Terminate();
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
    setAudioStatus(QString("Audio engine ready. Found %1 device(s). Select your microphone and output, then start passthrough.")
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
    startButton_->setEnabled(inputDeviceCombo_->currentData().isValid()
                             && outputDeviceCombo_->currentData().isValid());
}

void MainWindow::startAudio()
{
    if (!inputDeviceCombo_->currentData().isValid() || !outputDeviceCombo_->currentData().isValid()) {
        setAudioStatus("Choose an available microphone and output device first.", true);
        return;
    }
    QString errorMessage;
    const auto inputDevice = static_cast<PaDeviceIndex>(inputDeviceCombo_->currentData().toInt());
    const auto outputDevice = static_cast<PaDeviceIndex>(outputDeviceCombo_->currentData().toInt());
    if (!audioEngine_.start(inputDevice, outputDevice, &errorMessage)) {
        setAudioStatus(errorMessage, true);
        return;
    }
    inputDeviceCombo_->setEnabled(false);
    outputDeviceCombo_->setEnabled(false);
    startButton_->setEnabled(false);
    stopButton_->setEnabled(true);
    meterTimer_->start();
    setAudioStatus("Voice processing is running. Use headphones to prevent feedback. The current pitch/formant settings are initial tuning values.");
}

void MainWindow::stopAudio()
{
    meterTimer_->stop();
    audioEngine_.stop();
    inputMeter_->setValue(0);
    inputDeviceCombo_->setEnabled(true);
    outputDeviceCombo_->setEnabled(true);
    startButton_->setEnabled(inputDeviceCombo_->currentData().isValid()
                             && outputDeviceCombo_->currentData().isValid());
    stopButton_->setEnabled(false);
    setAudioStatus("Voice processing stopped.");
}

void MainWindow::updateInputMeter()
{
    inputMeter_->setValue(static_cast<int>(audioEngine_.inputLevel() * 100.0F));
}

void MainWindow::setAudioStatus(const QString &message, bool isError)
{
    audioStatusLabel_->setText(message);
    audioStatusLabel_->setStyleSheet(isError ? "color: #b42318;" : "color: palette(text);");
}
