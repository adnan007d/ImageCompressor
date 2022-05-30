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
    void doneWriting();

public slots:
    void renderImageCards(QVector<QImage> _images, qint64 size);
    void on_OpenButtonPressed();
    void on_SaveButtonPressed();
    void writeFinished(qint64 size);

private:
    void InitComponents();
    void resizeEvent(QResizeEvent *event);
    void InitSignalSlots();
    QString getFileSizeInUnits(const qint64 &size);

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

    FlowLayout *leftFlowLayout = nullptr;
    QScrollArea *leftScrollArea = nullptr;
    QWidget *leftScrollAreaWidget = nullptr;
    QLabel *fileSizeLabel = nullptr;

    QFrame *frameRight = nullptr;
    QVBoxLayout *rightFrameLayout = nullptr;
    QLabel *dataLabel = nullptr;
    QMovie *loadingGif = nullptr; // loading gif


    void clearLeftFrame();
    void clearRightFrame();
    void clearVariables();
    void clearEverything();

    void setLoading(bool loading);

    QVector<QImage> images{};
    QStringList fileNames{};

    qint64 initialSize = 0;

    const int defaultValue = 80;
    const char defaultValueS[3] = "80";
};
#endif // MAINWINDOW_H
