#include "imagecard.h"
#include "imagereader.h"
#include "imagewriter.h"
#include "convertimage.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QIntValidator>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumWidth(1000);

    this->setWindowIcon(QIcon(":icon.jpg"));
    this->setWindowTitle("Image Compressor");

    loadingGif = new QMovie(":loading.gif", "gif", this);

    InitComponents();
    InitSignalSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitComponents()
{
    InitLeftFrame();

    InitMidFrame();

    InitRightFrame();

    InitControlFrame();

    InitActionFrame();
}

void MainWindow::InitSignalSlots()
{
    connect(valueSlider, &QSlider::valueChanged, this, [this](int n)
            { compressInput->setText(QString::number(n)); });
    connect(compressInput, &QLineEdit::textChanged, this, [this](const QString &n)
            { valueSlider->setValue(n.toInt()); });

    connect(openButton, &QPushButton::clicked, this, &MainWindow::on_OpenButtonPressed);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::on_SaveButtonPressed);
    connect(convertButton, &QPushButton::clicked, this, &MainWindow::on_ConvertButtonPressed);

    // Selecting Destination Directory
    connect(fileDialogButton, &QPushButton::clicked, this,
            [this]()
            {
                QString path = QFileDialog::getExistingDirectory(this, "Select Destination Folder", filePathInput->text());
                if (!path.isNull())
                {
                    filePathInput->setText(std::move(path));
                }
            });
}

void MainWindow::InitLeftFrame()
{
    frameLeft = new QFrame(this);

    leftScrollArea = new QScrollArea(frameLeft);
    leftScrollAreaWidget = new QWidget(leftScrollArea);

    leftScrollArea->setWidgetResizable(true);

    leftFlowLayout = new FlowLayout(leftScrollAreaWidget);

    leftScrollArea->setWidget(leftScrollAreaWidget);

    fileSizeLabel = new QLabel(this);
    fileSizeLabel->setAlignment(Qt::AlignCenter);
}

void MainWindow::InitMidFrame()
{
    frameMid = new QFrame(this);
    midFrameLayout = new QVBoxLayout(frameMid);
}

void MainWindow::InitRightFrame()
{
    frameRight = new QFrame(this);

    rightScrollArea = new QScrollArea(frameRight);
    rightScrollAreaWidget = new QWidget(rightScrollArea);

    rightScrollArea->setWidgetResizable(true);

    rightFlowLayout = new FlowLayout(rightScrollAreaWidget);

    rightScrollArea->setWidget(rightScrollAreaWidget);
}

void MainWindow::InitControlFrame()
{
    controlWidget = new QWidget(this);
    controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel(tr("Adjust the compression value (0-99)"), this);

    valueSlider = new QSlider(Qt::Horizontal, this);
    valueSlider->setValue(defaultValue);

    compressInput = new QLineEdit(tr(defaultValueS), this);
    compressInput->setValidator(new QIntValidator(0, 99, this));
    compressInput->setFixedWidth(30);

    pngCheckBox = new QCheckBox("Convert png to jpg", this);

    convertButton = new QPushButton("convert", this);
    convertButton->setEnabled(false);

    controlLayout->addStretch();
    controlLayout->addWidget(label);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(valueSlider);
    controlLayout->addWidget(compressInput);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(pngCheckBox);
    controlLayout->addWidget(convertButton);
    controlLayout->addStretch();
}

void MainWindow::InitActionFrame()
{
    // Action Buttons;
    actionWidget = new QWidget(this);
    actionLayout = new QHBoxLayout(actionWidget);

    openButton = new QPushButton(actionWidget);
    saveButton = new QPushButton(actionWidget);
    fileDialogButton = new QPushButton(actionWidget);

    openButton->setIcon(QIcon::fromTheme("image", QIcon(":image.png")));
    openButton->setText(tr("Open"));
    fileDialogButton->setIcon(QIcon::fromTheme("folder", QIcon(":folder.png")));
    saveButton->setIcon(QIcon::fromTheme("folder", QIcon(":folder.png")));
    saveButton->setText(tr("Save"));
    saveButton->setEnabled(false); // Disabled by default;

    filePathInput = new QLineEdit(QDir::homePath(), this);
    filePathInput->setReadOnly(true);

    actionLayout->addWidget(openButton);
    actionLayout->addStretch();
    actionLayout->addWidget(saveButton);
    actionLayout->addWidget(filePathInput);
    actionLayout->addWidget(fileDialogButton);
}

void MainWindow::clearLeftFrame()
{
    QLayoutItem *item;
    while ((item = leftFlowLayout->takeAt(0)))
    {
        delete item->widget();
        delete item;
    }
}

void MainWindow::clearMidFrame()
{
    QLayoutItem *item;
    while ((item = midFrameLayout->takeAt(0)))
    {
        delete item->widget();
        delete item;
    }
}

void MainWindow::clearRightFrame()
{
    QLayoutItem *item;
    while ((item = rightFlowLayout->takeAt(0)))
    {
        delete item->widget();
        delete item;
    }
}

void MainWindow::clearVariables()
{
    initialSize = 0;
    images.clear();
    fileNames.clear();
    pngCheckBox->setChecked(false);
    saveButton->setEnabled(false);
}

void MainWindow::clearEverything()
{
    clearVariables();
    clearMidFrame();
    clearLeftFrame();
    clearRightFrame();
}

void MainWindow::renderImageCards(std::vector<ImageConfig> imageConfigs, qint64 size)
{
    initialSize = std::move(size);

    for (auto &[imageMat, size] : imageConfigs)
    {
        ImageCard *card = new ImageCard(
            QImage((uchar *)imageMat.data, imageMat.cols, imageMat.rows, imageMat.step1(), QImage::Format_RGB888).rgbSwapped(),
            getFileSizeInUnits(size),
            frameLeft);
        leftFlowLayout->addWidget(card);
        images.push_back(std::move(imageMat));
    }
    fileSizeLabel->setText(getFileSizeInUnits(initialSize));
    setLoading(false);
    convertButton->setEnabled(true);
    emit doneReading();
}

void MainWindow::on_OpenButtonPressed()
{
    QStringList _fileNames = QFileDialog::getOpenFileNames(this, "Open Image Files", QDir::homePath(), "Image Files *.png *.jpg *.jpeg");
    if (!_fileNames.isEmpty())
    {
        clearEverything();
        saveButton->setEnabled(false);

        fileNames = std::move(_fileNames);
        QThread *imageThread = new QThread;
        ImageReader *readerWorker = new ImageReader(fileNames);

        readerWorker->moveToThread(imageThread);

        connect(imageThread, &QThread::started, readerWorker, &ImageReader::start);
        connect(readerWorker, &ImageReader::finished, this, &MainWindow::renderImageCards);
        connect(this, &MainWindow::doneReading, imageThread, &QThread::quit);
        connect(imageThread, &QThread::finished, readerWorker, &ImageReader::deleteLater);
        setLoading(true);
        imageThread->start();
    }
}

void MainWindow::on_SaveButtonPressed()
{
    // clearRightFrame();
    // QThread *imageThread = new QThread;
    // ImageWriter *writerWorker = new ImageWriter({
    //     images,
    //     fileNames,
    //     filePathInput->text(),
    //     100 - valueSlider->value(),
    //     pngCheckBox->isChecked(),
    // });

    // writerWorker->moveToThread(imageThread);
    // connect(imageThread, &QThread::started, writerWorker, &ImageWriter::start);
    // connect(writerWorker, &ImageWriter::finished, this, &MainWindow::writeFinished);
    // connect(this, &MainWindow::doneWriting, imageThread, &QThread::quit);
    // connect(imageThread, &QThread::finished, writerWorker, &ImageWriter::deleteLater);
    // setLoading(true);
    // imageThread->start();
}

void MainWindow::on_ConvertButtonPressed()
{
    clearRightFrame();
    QThread *imageThread = new QThread;
    ConvertImage *convertWorker = new ConvertImage(ConvertOptions{
        images,
        fileNames,
        100 - valueSlider->value(),
        pngCheckBox->isChecked()});

    convertWorker->moveToThread(imageThread);
    connect(imageThread, &QThread::started, convertWorker, &ConvertImage::start);
    connect(convertWorker, &ConvertImage::finished, this, &MainWindow::on_ConvertFinished);
    connect(this, &MainWindow::doneConverting, imageThread, &QThread::quit);
    connect(imageThread, &QThread::finished, convertWorker, &ConvertImage::deleteLater);
    setLoading(true);
    imageThread->start();
}

void MainWindow::on_ConvertFinished(std::vector<std::vector<uchar>> convertedBuffer)
{
    int i = 0;
    for (const auto &buffer : convertedBuffer)
    {
        QImage img = QImage::fromData(buffer.data(), buffer.size());
        ImageCard *card = new ImageCard(
            img,
            getFileSizeInUnits(buffer.size()),
            frameRight);
        rightFlowLayout->addWidget(card);
        ++i;
    }
    emit doneConverting();
    // fileSizeLabel->setText(getFileSizeInUnits(initialSize));
    setLoading(false);
    saveButton->setEnabled(true); // Enabling the save button
}

void MainWindow::writeFinished(qint64 size)
{
    // setLoading(false);

    // emit doneWriting();
    // dataLabel = new QLabel(frameRight);
    // dataLabel->setText(
    //     "<b>Compression Done Successfully</b><br /> Saved to <a href='file://" +
    //     filePathInput->text() + "'>" + filePathInput->text() + "</a><br />previous size = " +
    //     getFileSizeInUnits(initialSize) + "<br /> new size = " +
    //     getFileSizeInUnits(size) + "<br />Reduced: <b>" +
    //     getFileSizeInUnits(initialSize - size) + "</b>");

    // QPushButton *resultOpenButton = new QPushButton("Open Folder", frameRight);

    // connect(resultOpenButton, &QPushButton::clicked, this, [this]()
    //         { QDesktopServices::openUrl(QUrl::fromLocalFile(filePathInput->text())); });

    // midFrameLayout->addWidget(dataLabel);
    // midFrameLayout->addWidget(resultOpenButton);
}

inline QString MainWindow::getFileSizeInUnits(const qint64 &size)
{
    const double mb = 1000000.0;
    if (size > mb)
        return QString::number(size / mb) + " MB";
    else
        return QString::number(size * 1000 / mb) + " KB";
}

void MainWindow::setLoading(bool loading)
{
    if (loading)
    {
        openButton->setEnabled(false);
        saveButton->setEnabled(false);
        fileDialogButton->setEnabled(false);
        convertButton->setEnabled(false);

        qDebug() << "Loading...";
        dataLabel = new QLabel(frameMid);
        dataLabel->setFixedSize(frameMid->width(), frameMid->width());
        dataLabel->setScaledContents(true);
        dataLabel->setAlignment(Qt::AlignCenter);

        dataLabel->setText("Loading...");
        dataLabel->setMovie(loadingGif);
        midFrameLayout->addWidget(dataLabel, 0, Qt::AlignCenter);
        loadingGif->start();
    }
    else
    {
        openButton->setEnabled(true);
        saveButton->setEnabled(true);
        fileDialogButton->setEnabled(true);
        convertButton->setEnabled(true);

        loadingGif->stop();
        dataLabel->setText("");
        qDebug() << "Not Loading";
        midFrameLayout->removeWidget(dataLabel);
        delete dataLabel;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    const int width = this->width();
    const int height = this->height();

    const int _5Height = (5 * height) / 100;
    const int _5Width = (5 * width) / 100;

    // 35% width 5% * 7 = 35%
    const int frameWidth = _5Width * 7;
    // 70% height
    const int frameHeight = _5Height * 14;

    frameLeft->setGeometry(_5Width, 0, frameWidth, frameHeight);
    leftScrollArea->setGeometry(0, 0, frameWidth, frameHeight);

    fileSizeLabel->setGeometry(_5Width, frameHeight, frameWidth, 50);

    frameRight->setGeometry(width - _5Width * 7 - _5Width, 0, frameWidth, frameHeight);
    rightScrollArea->setGeometry(0, 0, frameWidth, frameHeight);

    const int midFrameWidth = _5Width * 3;
    const int midFrameHeight = _5Width * 4;

    frameMid->setGeometry(frameWidth + _5Width * 1.5, height / 2 - midFrameHeight, midFrameWidth, midFrameHeight);

    controlWidget->setGeometry(0, _5Height * 15, width, 100);
    actionWidget->setGeometry(_5Width, _5Height * 17, width - _5Width * 2, 100);
}
