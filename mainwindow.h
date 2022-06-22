#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "flowlayout.h"

#include <QSlider>
#include <QFrame>
#include <QMainWindow>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QMovie>
#include <QCheckBox>
#include "imageconfig.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void doneReading();
    void doneConverting();
    void doneWriting();

public slots:
    void renderImageCards(std::vector<ImageConfig> _images, qint64 size);
    void on_OpenButtonPressed();
    void on_SaveButtonPressed();
    void on_ConvertButtonPressed();
    void on_ConvertFinished(std::vector<std::vector<uchar>> convertedBuffer);
    void on_WriteFinished();

private:
    void InitComponents();
    void resizeEvent(QResizeEvent *event);
    void InitSignalSlots();
    inline QString getFileSizeInUnits(const qint64 &size);
    void InitLeftFrame();
    void InitMidFrame();
    void InitRightFrame();
    void InitControlFrame();
    void InitActionFrame();

    Ui::MainWindow *ui;

    QFrame *frameLeft = nullptr;

    QSlider *valueSlider = nullptr;
    QLineEdit *compressInput = nullptr;
    QHBoxLayout *controlLayout = nullptr;
    QWidget *controlWidget = nullptr;

    QHBoxLayout *actionLayout = nullptr;
    QWidget *actionWidget = nullptr;
    QPushButton *openButton = nullptr;
    QPushButton *saveButton = nullptr;
    QLineEdit *filePathInput = nullptr;
    QPushButton *fileDialogButton = nullptr;
    QCheckBox *pngCheckBox = nullptr;
    QPushButton *convertButton = nullptr;

    FlowLayout *leftFlowLayout = nullptr;
    QScrollArea *leftScrollArea = nullptr;
    QWidget *leftScrollAreaWidget = nullptr;
    QLabel *fileSizeLabel = nullptr;

    QFrame *frameRight = nullptr;
    FlowLayout *rightFlowLayout = nullptr;
    QScrollArea *rightScrollArea = nullptr;
    QWidget *rightScrollAreaWidget = nullptr;

    QFrame *frameMid = nullptr;
    QVBoxLayout *midFrameLayout = nullptr;
    QLabel *dataLabel = nullptr;
    QMovie *loadingGif = nullptr;

    void clearLeftFrame();
    void clearMidFrame();
    void clearRightFrame();
    void clearVariables();
    void clearEverything();

    void setLoading(bool loading);

    std::vector<cv::Mat> images{};
    std::vector<std::vector<uchar>> convertedBuffers{};
    QStringList fileNames{};

    qint64 initialSize = 0;
    qint64 convertedSize = 0;

    const int defaultValue = 80;
    const char defaultValueS[3] = "80";
};
#endif // MAINWINDOW_H
