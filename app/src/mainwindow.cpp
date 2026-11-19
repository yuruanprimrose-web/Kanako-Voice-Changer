#include "mainwindow.h"
#include <portaudio.h>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSignalBlocker>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Kanako Voice Changer");
    setMinimumSize(680, 650);
    resize(800, 720);
    setStyleSheet(R"(
        QMainWindow { background: #10131b; }
        QWidget { color: #edf0f7; font-family: "Segoe UI"; font-size: 10pt; }
        QLabel#brandTitle { color: #ffffff; font-size: 25pt; font-weight: 700; letter-spacing: 1px; }
        QLabel#brandSubtitle { color: #aab4c8; font-size: 11pt; }
        QGroupBox { border: 1px solid #30394b; border-radius: 12px; margin-top: 14px; padding: 18px 14px 12px 14px; font-weight: 600; color: #c9d5ea; }
        QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }
        QComboBox { background: #1a2130; border: 1px solid #3b465b; border-radius: 7px; padding: 9px 10px; min-height: 20px; selection-background-color: #6855d9; }
        QComboBox QAbstractItemView { background: #1a2130; selection-background-color: #6855d9; }
        QPushButton { background: #252d3d; border: 1px solid #414d64; border-radius: 8px; padding: 11px 16px; font-weight: 600; }
        QPushButton:hover { background: #303b50; }
        QPushButton:disabled { color: #70798a; background: #1a202c; border-color: #2c3443; }
        QPushButton#startButton { background: #6855d9; border-color: #7868e4; color: white; }
        QPushButton#startButton:hover { background: #7968e9; }
        QPushButton#stopButton { background: #672f3b; border-color: #8b4656; }
        QSlider::groove:horizontal { height: 7px; background: #30394b; border-radius: 3px; }
        QSlider::sub-page:horizontal { background: #6855d9; border-radius: 3px; }
        QSlider::handle:horizontal { background: #ffffff; border: 2px solid #6855d9; width: 16px; margin: -6px 0; border-radius: 9px; }
        QProgressBar { background: #1a2130; border: 1px solid #30394b; border-radius: 5px; min-height: 12px; max-height: 12px; }
        QProgressBar::chunk { background: #37c99a; border-radius: 4px; }
        QLabel#statusLabel { background: #1a2130; border: 1px solid #30394b; border-radius: 8px; padding: 10px; color: #b9c6dc; }
        QLabel#helperText { color: #8f9bb0; font-size: 9pt; }
    )");

    auto *central = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(30, 26, 30, 26);
    rootLayout->setSpacing(16);

    auto *heading = new QLabel("KANAKO", central);
    heading->setObjectName("brandTitle");
    auto *subtitle = new QLabel("VOICE CHANGER  /  REAL-TIME AUDIO", central);
    subtitle->setObjectName("brandSubtitle");

    auto *deviceGroup = new QGroupBox("AUDIO DEVICES", central);
    auto *deviceLayout = new QFormLayout(deviceGroup);
    deviceLayout->setVerticalSpacing(12);
    inputDeviceCombo_ = new QComboBox(deviceGroup);
    inputDeviceCombo_->setObjectName("inputDeviceCombo");
    outputDeviceCombo_ = new QComboBox(deviceGroup);
    outputDeviceCombo_->setObjectName("outputDeviceCombo");
    deviceLayout->addRow("Microphone", inputDeviceCombo_);
    deviceLayout->addRow("Speaker / Output", outputDeviceCombo_);

    auto *presetGroup = new QGroupBox("VOICE PRESET", central);
    auto *presetLayout = new QFormLayout(presetGroup);
    presetCombo_ = new QComboBox(presetGroup);
    presetCombo_->setObjectName("voicePresetCombo");
    presetCombo_->addItem("Neutral / Bypass", "neutral");
    presetCombo_->addItem("Female-style", "female");
    presetCombo_->addItem("Male-style", "male");
    presetLayout->addRow("Choose your voice", presetCombo_);

    auto *gainGroup = new QGroupBox("INPUT & OUTPUT LEVELS", central);
    auto *gainLayout = new QFormLayout(gainGroup);
    microphoneGainSlider_ = new QSlider(Qt::Horizontal, gainGroup);
    microphoneGainSlider_->setObjectName("microphoneGainSlider");
    microphoneGainSlider_->setRange(0, 300);
    microphoneGainSlider_->setSingleStep(5);
    microphoneGainSlider_->setValue(100);
    microphoneGainValue_ = new QLabel("100%", gainGroup);
    auto *micGainRow = new QWidget(gainGroup);
    auto *micGainRowLayout = new QHBoxLayout(micGainRow);
    micGainRowLayout->setContentsMargins(0, 0, 0, 0);
    micGainRowLayout->addWidget(microphoneGainSlider_, 1);
    micGainRowLayout->addWidget(microphoneGainValue_);
    gainLayout->addRow("Microphone gain", micGainRow);

    outputGainSlider_ = new QSlider(Qt::Horizontal, gainGroup);
    outputGainSlider_->setObjectName("outputGainSlider");
    outputGainSlider_->setRange(0, 300);
    outputGainSlider_->setSingleStep(5);
    outputGainSlider_->setValue(100);
    outputGainValue_ = new QLabel("100%", gainGroup);
    auto *outputGainRow = new QWidget(gainGroup);
    auto *outputGainRowLayout = new QHBoxLayout(outputGainRow);
    outputGainRowLayout->setContentsMargins(0, 0, 0, 0);
    outputGainRowLayout->addWidget(outputGainSlider_, 1);
    outputGainRowLayout->addWidget(outputGainValue_);
    gainLayout->addRow("Speaker / output volume", outputGainRow);

    auto *gainNote = new QLabel("Increase microphone gain if your input is quiet. Increase output volume if the processed voice is quiet. High levels may cause distortion.", gainGroup);
    gainNote->setObjectName("helperText");
    gainNote->setWordWrap(true);
    gainLayout->addRow(gainNote);

    auto *meterLabel = new QLabel("MICROPHONE INPUT LEVEL", central);
    meterLabel->setStyleSheet("color: #aab4c8; font-size: 9pt; font-weight: 600;");
    inputMeter_ = new QProgressBar(central);
    inputMeter_->setObjectName("inputLevelMeter");
    inputMeter_->setRange(0, 100);
    inputMeter_->setValue(0);
    inputMeter_->setTextVisible(false);

    audioStatusLabel_ = new QLabel("Initializing audio devices...", central);
    audioStatusLabel_->setObjectName("statusLabel");
    audioStatusLabel_->setWordWrap(true);

    refreshDevicesButton_ = new QPushButton("Refresh devices", central);
    refreshDevicesButton_->setObjectName("refreshDevicesButton");
    startButton_ = new QPushButton("Start voice changer", central);
    startButton_->setObjectName("startButton");
    stopButton_ = new QPushButton("Stop", central);
    stopButton_->setObjectName("stopButton");
    stopButton_->setEnabled(false);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(refreshDevicesButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(startButton_, 2);
    buttonLayout->addWidget(stopButton_, 1);

    auto *note = new QLabel(
        "Tip: Use headphones to prevent feedback. Voice presets are local DSP effects; the final sound depends on your microphone and voice.",
        central);
    note->setObjectName("helperText");
    note->setWordWrap(true);

    rootLayout->addWidget(heading);
    rootLayout->addWidget(subtitle);
    rootLayout->addWidget(deviceGroup);
    rootLayout->addWidget(presetGroup);
    rootLayout->addWidget(gainGroup);
    rootLayout->addWidget(meterLabel);
    rootLayout->addWidget(inputMeter_);
    rootLayout->addWidget(audioStatusLabel_);
    rootLayout->addLayout(buttonLayout);
    rootLayout->addStretch();
    rootLayout->addWidget(note);
    setCentralWidget(central);

    connect(presetCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        audioEngine_.setPreset(static_cast<VoiceProcessor::Preset>(index));
        saveSettings();
    });
    connect(microphoneGainSlider_, &QSlider::valueChanged, this, [this](int value) {
        audioEngine_.setMicrophoneGain(static_cast<float>(value) / 100.0F);
        microphoneGainValue_->setText(QString("%1%").arg(value));
        saveSettings();
    });
    connect(outputGainSlider_, &QSlider::valueChanged, this, [this](int value) {
        audioEngine_.setOutputGain(static_cast<float>(value) / 100.0F);
        outputGainValue_->setText(QString("%1%").arg(value));
        saveSettings();
    });
    connect(inputDeviceCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { saveSettings(); });
    connect(outputDeviceCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { saveSettings(); });
    connect(refreshDevicesButton_, &QPushButton::clicked, this, [this]() {
        populateDeviceLists();
        restoreSettings();
        setAudioStatus("Audio device list refreshed.");
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
    restoreSettings();
    const PaDeviceIndex deviceCount = Pa_GetDeviceCount();
    if (deviceCount < 0) {
        setAudioStatus(QString("Could not enumerate audio devices: %1").arg(Pa_GetErrorText(deviceCount)), true);
        return;
    }
    setAudioStatus(QString("Audio engine ready. Found %1 device(s). Choose a voice preset and start.")
                       .arg(deviceCount));
}

void MainWindow::populateDeviceLists()
{
    const QSignalBlocker inputSignals(inputDeviceCombo_);
    const QSignalBlocker outputSignals(outputDeviceCombo_);
    inputDeviceCombo_->clear();
    outputDeviceCombo_->clear();
    const PaDeviceIndex deviceCount = Pa_GetDeviceCount();
    if (deviceCount < 0) {
        inputDeviceCombo_->addItem("Audio devices unavailable");
        outputDeviceCombo_->addItem("Audio devices unavailable");
        startButton_->setEnabled(false);
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

void MainWindow::restoreSettings()
{
    QSettings settings("Yuruan Primrose", "Kanako Voice Changer");
    const int inputIndex = inputDeviceCombo_->findText(settings.value("audio/inputDevice").toString());
    const int outputIndex = outputDeviceCombo_->findText(settings.value("audio/outputDevice").toString());
    const int presetIndex = presetCombo_->findData(settings.value("audio/voicePreset", "neutral"));
    const int microphoneGain = settings.value("audio/microphoneGain", 100).toInt();
    const int outputGain = settings.value("audio/outputGain", 100).toInt();
    if (inputIndex >= 0) inputDeviceCombo_->setCurrentIndex(inputIndex);
    if (outputIndex >= 0) outputDeviceCombo_->setCurrentIndex(outputIndex);
    if (presetIndex >= 0) presetCombo_->setCurrentIndex(presetIndex);
    microphoneGainSlider_->setValue(qBound(0, microphoneGain, 300));
    outputGainSlider_->setValue(qBound(0, outputGain, 300));
    audioEngine_.setMicrophoneGain(static_cast<float>(microphoneGainSlider_->value()) / 100.0F);
    audioEngine_.setOutputGain(static_cast<float>(outputGainSlider_->value()) / 100.0F);
    microphoneGainValue_->setText(QString("%1%").arg(microphoneGainSlider_->value()));
    outputGainValue_->setText(QString("%1%").arg(outputGainSlider_->value()));
}

void MainWindow::saveSettings()
{
    QSettings settings("Yuruan Primrose", "Kanako Voice Changer");
    if (inputDeviceCombo_->currentData().isValid())
        settings.setValue("audio/inputDevice", inputDeviceCombo_->currentText());
    if (outputDeviceCombo_->currentData().isValid())
        settings.setValue("audio/outputDevice", outputDeviceCombo_->currentText());
    settings.setValue("audio/voicePreset", presetCombo_->currentData().toString());
    settings.setValue("audio/microphoneGain", microphoneGainSlider_->value());
    settings.setValue("audio/outputGain", outputGainSlider_->value());
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
    refreshDevicesButton_->setEnabled(false);
    startButton_->setEnabled(false);
    stopButton_->setEnabled(true);
    meterTimer_->start();
    setAudioStatus("Voice processing is running. Use headphones to prevent feedback.");
}

void MainWindow::stopAudio()
{
    meterTimer_->stop();
    audioEngine_.stop();
    inputMeter_->setValue(0);
    inputDeviceCombo_->setEnabled(true);
    outputDeviceCombo_->setEnabled(true);
    refreshDevicesButton_->setEnabled(true);
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
    audioStatusLabel_->setStyleSheet(isError
        ? "QLabel#statusLabel { color: #ff8b8b; border-color: #9b4545; }"
        : "QLabel#statusLabel { color: #b9c6dc; border-color: #30394b; }");
}
