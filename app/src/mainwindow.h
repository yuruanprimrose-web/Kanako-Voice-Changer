#pragma once
#include <QMainWindow>
class QComboBox;
class QLabel;
class QPushButton;

class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
private:
    void initializeAudioDevices();
    void populateDeviceLists();
    void setAudioStatus(const QString &message, bool isError = false);
    QComboBox *inputDeviceCombo_ = nullptr;
    QComboBox *outputDeviceCombo_ = nullptr;
    QLabel *audioStatusLabel_ = nullptr;
    QLabel *inputMeterLabel_ = nullptr;
    QPushButton *startButton_ = nullptr;
    bool portAudioInitialized_ = false;
};
