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

private:
    void InitComponents();
    void resizeEvent(QResizeEvent *event);
    void InitSignalSlots();
    void renderCards(const QStringList &fileNames);


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
    QLineEdit  *filePathInput = nullptr;
    QPushButton *fileDialogButton = nullptr;

    FlowLayout *leftFlowLayout = nullptr;
    QScrollArea *leftScrollArea = nullptr;
    QWidget *leftScrollAreaWidget = nullptr;

    QFrame *frameRight = nullptr;
    QVBoxLayout *rightFrameLayout = nullptr;
    QLabel *dataLabel = nullptr;
    QMovie *loadingGif = nullptr; // loading gif

    void setLoading(bool loading);


//    FlowLayout *rightFlowLayout = nullptr;
//    QScrollArea *rightScrollArea = nullptr;
//    QWidget *rightScrollAreaWidget = nullptr;

    QVector<QImage> images{};
    QStringList fileNames{};

    uint64_t initialSize = 0;

    int defaultValue = 80;
    char defaultValueS[3] = "80";

};
#endif // MAINWINDOW_H
