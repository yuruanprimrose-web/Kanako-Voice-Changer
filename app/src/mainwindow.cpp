#include "mainwindow.h"
#include <portaudio.h>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QFileDialog>
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
    setMinimumSize(700, 800);
    resize(820, 920);
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
        QPushButton#recordButton { background: #a83245; border-color: #c34b5e; color: white; }
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
    virtualMicDeviceCombo_ = new QComboBox(deviceGroup);
    virtualMicDeviceCombo_->setObjectName("virtualMicDeviceCombo");
    deviceLayout->addRow("Virtual microphone", virtualMicDeviceCombo_);
    auto *virtualMicNote = new QLabel(
        "Optional: install VB-CABLE first. Kanako sends audio to CABLE Input; choose CABLE Output as the microphone in Discord, Zoom, or OBS.",
        deviceGroup);
    virtualMicNote->setObjectName("helperText");
    virtualMicNote->setWordWrap(true);
    deviceLayout->addRow(virtualMicNote);

    auto *presetGroup = new QGroupBox("VOICE PRESET", central);
    auto *presetLayout = new QFormLayout(presetGroup);
    presetCombo_ = new QComboBox(presetGroup);
    presetCombo_->setObjectName("voicePresetCombo");
    presetCombo_->addItem("Neutral / Bypass", "neutral");
    presetCombo_->addItem("Female-style", "female");
    presetCombo_->addItem("Male-style", "male");
    presetLayout->addRow("Choose your voice", presetCombo_);

    auto *toneGroup = new QGroupBox("VOICE TONE TUNING", central);
    auto *toneLayout = new QFormLayout(toneGroup);
    pitchSlider_ = new QSlider(Qt::Horizontal, toneGroup);
    pitchSlider_->setObjectName("pitchToneSlider");
    pitchSlider_->setRange(-120, 120);
    pitchSlider_->setSingleStep(1);
    pitchSlider_->setValue(0);
    pitchValue_ = new QLabel("0.0 st", toneGroup);
    auto *pitchRow = new QWidget(toneGroup);
    auto *pitchRowLayout = new QHBoxLayout(pitchRow);
    pitchRowLayout->setContentsMargins(0, 0, 0, 0);
    pitchRowLayout->addWidget(pitchSlider_, 1);
    pitchRowLayout->addWidget(pitchValue_);
    toneLayout->addRow("Pitch", pitchRow);

    formantSlider_ = new QSlider(Qt::Horizontal, toneGroup);
    formantSlider_->setObjectName("formantToneSlider");
    formantSlider_->setRange(-60, 60);
    formantSlider_->setSingleStep(1);
    formantSlider_->setValue(0);
    formantValue_ = new QLabel("0.0 st", toneGroup);
    auto *formantRow = new QWidget(toneGroup);
    auto *formantRowLayout = new QHBoxLayout(formantRow);
    formantRowLayout->setContentsMargins(0, 0, 0, 0);
    formantRowLayout->addWidget(formantSlider_, 1);
    formantRowLayout->addWidget(formantValue_);
    toneLayout->addRow("Formant / vocal character", formantRow);

    auto *toneNote = new QLabel("Start with a preset, then fine-tune pitch and vocal character. Small adjustments usually sound more natural.", toneGroup);
    toneNote->setObjectName("helperText");
    toneNote->setWordWrap(true);
    toneLayout->addRow(toneNote);

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

    auto *recordGroup = new QGroupBox("VOICE RECORDING", central);
    auto *recordLayout = new QVBoxLayout(recordGroup);
    auto *recordButtons = new QHBoxLayout();
    recordButton_ = new QPushButton("Record WAV", recordGroup);
    recordButton_->setObjectName("recordButton");
    recordButton_->setEnabled(false);
    pauseRecordingButton_ = new QPushButton("Pause", recordGroup);
    pauseRecordingButton_->setEnabled(false);
    stopRecordingButton_ = new QPushButton("Stop recording", recordGroup);
    stopRecordingButton_->setEnabled(false);
    recordButtons->addWidget(recordButton_);
    recordButtons->addWidget(pauseRecordingButton_);
    recordButtons->addWidget(stopRecordingButton_);
    recordingStatusLabel_ = new QLabel("Ready to record processed voice (WAV)", recordGroup);
    recordingStatusLabel_->setObjectName("helperText");
    recordLayout->addLayout(recordButtons);
    recordLayout->addWidget(recordingStatusLabel_);

    auto *note = new QLabel(
        "Tip: Use headphones to prevent feedback. Voice presets are local DSP effects; the final sound depends on your microphone and voice.",
        central);
    note->setObjectName("helperText");
    note->setWordWrap(true);

    rootLayout->addWidget(heading);
    rootLayout->addWidget(subtitle);
    rootLayout->addWidget(deviceGroup);
    rootLayout->addWidget(presetGroup);
    rootLayout->addWidget(toneGroup);
    rootLayout->addWidget(gainGroup);
    rootLayout->addWidget(meterLabel);
    rootLayout->addWidget(inputMeter_);
    rootLayout->addWidget(audioStatusLabel_);
    rootLayout->addLayout(buttonLayout);
    rootLayout->addWidget(recordGroup);
    rootLayout->addStretch();
    rootLayout->addWidget(note);
    setCentralWidget(central);

    connect(presetCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        const int pitch = index == 1 ? 20 : (index == 2 ? -30 : 0);
        const int formant = index == 1 ? 10 : (index == 2 ? -15 : 0);
        {
            const QSignalBlocker pitchBlock(pitchSlider_);
            const QSignalBlocker formantBlock(formantSlider_);
            pitchSlider_->setValue(pitch);
            formantSlider_->setValue(formant);
        }
        audioEngine_.setVoiceTone(static_cast<float>(pitch) / 10.0F, static_cast<float>(formant) / 10.0F);
        pitchValue_->setText(QString("%1 st").arg(static_cast<double>(pitch) / 10.0, 0, 'f', 1));
        formantValue_->setText(QString("%1 st").arg(static_cast<double>(formant) / 10.0, 0, 'f', 1));
        saveSettings();
    });
    const auto updateTone = [this]() {
        const int pitch = pitchSlider_->value();
        const int formant = formantSlider_->value();
        audioEngine_.setVoiceTone(static_cast<float>(pitch) / 10.0F, static_cast<float>(formant) / 10.0F);
        pitchValue_->setText(QString("%1 st").arg(static_cast<double>(pitch) / 10.0, 0, 'f', 1));
        formantValue_->setText(QString("%1 st").arg(static_cast<double>(formant) / 10.0, 0, 'f', 1));
        saveSettings();
    };
    connect(pitchSlider_, &QSlider::valueChanged, this, [updateTone](int) { updateTone(); });
    connect(formantSlider_, &QSlider::valueChanged, this, [updateTone](int) { updateTone(); });
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
    connect(recordButton_, &QPushButton::clicked, this, &MainWindow::startRecording);
    connect(pauseRecordingButton_, &QPushButton::clicked, this, &MainWindow::pauseOrResumeRecording);
    connect(stopRecordingButton_, &QPushButton::clicked, this, &MainWindow::stopRecording);
    meterTimer_ = new QTimer(this);
    meterTimer_->setInterval(33);
    recordingTimer_ = new QTimer(this);
    recordingTimer_->setInterval(1000);
    connect(recordingTimer_, &QTimer::timeout, this, &MainWindow::updateRecordingTimer);
    connect(meterTimer_, &QTimer::timeout, this, &MainWindow::updateInputMeter);
    initializeAudioDevices();
}

MainWindow::~MainWindow()
{
    meterTimer_->stop();
    recordingTimer_->stop();
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
    const QSignalBlocker virtualSignals(virtualMicDeviceCombo_);
    inputDeviceCombo_->clear();
    outputDeviceCombo_->clear();
    virtualMicDeviceCombo_->clear();
    virtualMicDeviceCombo_->addItem("Disabled (optional)", static_cast<int>(paNoDevice));
    const PaDeviceIndex deviceCount = Pa_GetDeviceCount();
    if (deviceCount < 0) {
        inputDeviceCombo_->addItem("Audio devices unavailable");
        outputDeviceCombo_->addItem("Audio devices unavailable");
        virtualMicDeviceCombo_->setItemText(0, "Not detected — install VB-CABLE");
        startButton_->setEnabled(false);
        return;
    }
    for (PaDeviceIndex index = 0; index < deviceCount; ++index) {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(index);
        if (info == nullptr) continue;
        const QString deviceName = QString::fromUtf8(info->name);
        if (info->maxInputChannels > 0) inputDeviceCombo_->addItem(deviceName, index);
        if (info->maxOutputChannels > 0) {
            outputDeviceCombo_->addItem(deviceName, index);
            if (deviceName.contains("CABLE Input", Qt::CaseInsensitive))
                virtualMicDeviceCombo_->addItem(deviceName, index);
        }
    }
    if (inputDeviceCombo_->count() == 0) inputDeviceCombo_->addItem("No microphone detected");
    if (outputDeviceCombo_->count() == 0) outputDeviceCombo_->addItem("No output device detected");
    if (virtualMicDeviceCombo_->count() == 1)
        virtualMicDeviceCombo_->setItemText(0, "Not detected — install VB-CABLE");
    startButton_->setEnabled(inputDeviceCombo_->currentData().isValid()
                             && outputDeviceCombo_->currentData().isValid());
}

void MainWindow::restoreSettings()
{
    QSettings settings("Yuruan Primrose", "Kanako Voice Changer");
    const int inputIndex = inputDeviceCombo_->findText(settings.value("audio/inputDevice").toString());
    const int outputIndex = outputDeviceCombo_->findText(settings.value("audio/outputDevice").toString());
    const int virtualIndex = virtualMicDeviceCombo_->findText(settings.value("audio/virtualMicDevice").toString());
    const int presetIndex = presetCombo_->findData(settings.value("audio/voicePreset", "neutral"));
    const int defaultPitch = presetIndex == 1 ? 20 : (presetIndex == 2 ? -30 : 0);
    const int defaultFormant = presetIndex == 1 ? 10 : (presetIndex == 2 ? -15 : 0);
    const int pitch = settings.value("audio/pitchTenths", defaultPitch).toInt();
    const int formant = settings.value("audio/formantTenths", defaultFormant).toInt();
    const int microphoneGain = settings.value("audio/microphoneGain", 100).toInt();
    const int outputGain = settings.value("audio/outputGain", 100).toInt();
    if (inputIndex >= 0) inputDeviceCombo_->setCurrentIndex(inputIndex);
    if (outputIndex >= 0) outputDeviceCombo_->setCurrentIndex(outputIndex);
    if (virtualIndex >= 0) virtualMicDeviceCombo_->setCurrentIndex(virtualIndex);
    if (presetIndex >= 0) presetCombo_->setCurrentIndex(presetIndex);
    {
        const QSignalBlocker pitchBlock(pitchSlider_);
        const QSignalBlocker formantBlock(formantSlider_);
        pitchSlider_->setValue(qBound(-120, pitch, 120));
        formantSlider_->setValue(qBound(-60, formant, 60));
    }
    audioEngine_.setVoiceTone(static_cast<float>(pitchSlider_->value()) / 10.0F,
                              static_cast<float>(formantSlider_->value()) / 10.0F);
    pitchValue_->setText(QString("%1 st").arg(static_cast<double>(pitchSlider_->value()) / 10.0, 0, 'f', 1));
    formantValue_->setText(QString("%1 st").arg(static_cast<double>(formantSlider_->value()) / 10.0, 0, 'f', 1));
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
    settings.setValue("audio/virtualMicDevice",
        virtualMicDeviceCombo_->currentData().toInt() == static_cast<int>(paNoDevice)
            ? QString() : virtualMicDeviceCombo_->currentText());
    settings.setValue("audio/voicePreset", presetCombo_->currentData().toString());
    settings.setValue("audio/pitchTenths", pitchSlider_->value());
    settings.setValue("audio/formantTenths", formantSlider_->value());
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
    const auto virtualMicDevice = static_cast<PaDeviceIndex>(virtualMicDeviceCombo_->currentData().toInt());
    if (!audioEngine_.start(inputDevice, outputDevice, virtualMicDevice, &errorMessage)) {
        setAudioStatus(errorMessage, true);
        return;
    }
    inputDeviceCombo_->setEnabled(false);
    outputDeviceCombo_->setEnabled(false);
    virtualMicDeviceCombo_->setEnabled(false);
    refreshDevicesButton_->setEnabled(false);
    startButton_->setEnabled(false);
    stopButton_->setEnabled(true);
    recordButton_->setEnabled(true);
    meterTimer_->start();
    setAudioStatus(virtualMicDevice != paNoDevice
        ? "Voice processing is running. Processed voice is also routed to the selected virtual microphone. Use headphones to prevent feedback."
        : "Voice processing is running. Use headphones to prevent feedback.");
}

void MainWindow::stopAudio()
{
    if (audioEngine_.isRecording()) stopRecording();
    const bool virtualMicWasActive = audioEngine_.isVirtualMicActive();
    const auto virtualMicDropped = audioEngine_.virtualMicDroppedSamples();
    const auto virtualMicUnderflow = audioEngine_.virtualMicUnderflowSamples();
    meterTimer_->stop();
    audioEngine_.stop();
    inputMeter_->setValue(0);
    inputDeviceCombo_->setEnabled(true);
    outputDeviceCombo_->setEnabled(true);
    virtualMicDeviceCombo_->setEnabled(true);
    refreshDevicesButton_->setEnabled(true);
    startButton_->setEnabled(inputDeviceCombo_->currentData().isValid()
                             && outputDeviceCombo_->currentData().isValid());
    stopButton_->setEnabled(false);
    recordButton_->setEnabled(false);
    if (virtualMicWasActive) {
        setAudioStatus(QString("Voice processing stopped. Virtual mic buffer: %1 dropped samples, %2 underflow samples (silence).")
                           .arg(virtualMicDropped).arg(virtualMicUnderflow),
                       virtualMicDropped > 0);
    } else {
        setAudioStatus("Voice processing stopped.");
    }
}

void MainWindow::startRecording()
{
    if (!audioEngine_.isRunning()) {
        setAudioStatus("Start the voice changer before recording.", true);
        return;
    }

    // Capture immediately to a temporary WAV. Ask for the destination only after Stop.
    recordingTempPath_ = QDir::tempPath() + "/Kanako-Recording-" +
                         QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss-zzz") + ".wav";
    QString error;
    if (!audioEngine_.startRecording(recordingTempPath_, &error)) {
        recordingTempPath_.clear();
        setAudioStatus(error, true);
        return;
    }

    recordingSeconds_ = 0;
    recordingStatusLabel_->setText("Recording processed voice • 00:00");
    recordButton_->setEnabled(false);
    pauseRecordingButton_->setEnabled(true);
    pauseRecordingButton_->setText("Pause");
    stopRecordingButton_->setEnabled(true);
    recordingTimer_->start();
    setAudioStatus("Recording started. Speak now; choose where to save after stopping.");
}

void MainWindow::pauseOrResumeRecording()
{
    if (!audioEngine_.isRecording()) return;
    if (audioEngine_.isRecordingPaused()) {
        audioEngine_.resumeRecording();
        pauseRecordingButton_->setText("Pause");
        recordingStatusLabel_->setText("Recording processed voice • resumed");
    } else {
        audioEngine_.pauseRecording();
        pauseRecordingButton_->setText("Resume");
        recordingStatusLabel_->setText("Recording paused");
    }
}

void MainWindow::stopRecording()
{
    if (!audioEngine_.isRecording()) return;
    recordingTimer_->stop();
    const auto dropped = audioEngine_.droppedRecordingSamples();
    const auto accepted = audioEngine_.acceptedRecordingSamples();
    const auto peak = audioEngine_.recordingPeakLevel();
    audioEngine_.stopRecording();

    recordButton_->setEnabled(audioEngine_.isRunning());
    pauseRecordingButton_->setEnabled(false);
    pauseRecordingButton_->setText("Pause");
    stopRecordingButton_->setEnabled(false);

    const QString defaultName = QDir::homePath() + "/Kanako Recording " +
                                QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss") + ".wav";
    QString destination = QFileDialog::getSaveFileName(this, "Save Kanako recording", defaultName,
                                                         "WAV audio (*.wav)");
    if (destination.isEmpty()) {
        QFile::remove(recordingTempPath_);
        recordingTempPath_.clear();
        recordingStatusLabel_->setText("Recording discarded");
        setAudioStatus("Recording stopped. No file was saved because the save dialog was cancelled.");
        return;
    }
    if (!destination.endsWith(".wav", Qt::CaseInsensitive)) destination += ".wav";

    if (QFile::exists(destination)) QFile::remove(destination);
    const bool copied = QFile::copy(recordingTempPath_, destination);
    QFile::remove(recordingTempPath_);
    recordingTempPath_.clear();

    if (!copied) {
        recordingStatusLabel_->setText("Could not save recording");
        setAudioStatus("The recording was captured, but Kanako could not copy it to the selected location. Check folder permissions and free space.", true);
        return;
    }

    recordingStatusLabel_->setText(QString("Recording saved • %1:%2")
        .arg(recordingSeconds_ / 60, 2, 10, QChar('0'))
        .arg(recordingSeconds_ % 60, 2, 10, QChar('0')));
    if (accepted == 0 || peak < 0.0001F) {
        setAudioStatus(QString("WAV file saved, but the recorder received no usable voice signal (samples: %1, peak: %2). Check the microphone input meter and selected device.")
                           .arg(accepted).arg(peak, 0, 'f', 6), true);
    } else if (dropped > 0) {
        setAudioStatus(QString("WAV saved with audio detected (peak %1), but %2 samples were dropped because the disk could not keep up.")
                           .arg(peak, 0, 'f', 4).arg(dropped), true);
    } else {
        setAudioStatus(QString("WAV recording saved. Audio detected (peak level: %1).")
                           .arg(peak, 0, 'f', 4));
    }
}

void MainWindow::updateRecordingTimer()
{
    if (!audioEngine_.isRecording() || audioEngine_.isRecordingPaused()) return;
    ++recordingSeconds_;
    recordingStatusLabel_->setText(QString("Recording processed voice • %1:%2")
        .arg(recordingSeconds_ / 60, 2, 10, QChar('0'))
        .arg(recordingSeconds_ % 60, 2, 10, QChar('0')));
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
