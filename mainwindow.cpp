#include "imagecard.h"
#include "imagereader.h"
#include "imagewriter.h"
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
    this->setMinimumWidth(850);

    loadingGif = new QMovie(":loading.gif", "gif", this);

    InitComponents();
    InitSignalSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::renderImageCards(QVector<QImage> _images, qint64 size)
{
    images = std::move(_images);
    initialSize = std::move(size);

    for (const auto &image : images)
    {
        ImageCard *card = new ImageCard(image, frameLeft);
        leftFlowLayout->addWidget(card);
    }
    fileSizeLabel->setText(getFileSizeInUnits(initialSize));
    setLoading(false);
    saveButton->setEnabled(true); // Enabling the save button
    emit doneReading();
}

void MainWindow::on_OpenButtonPressed()
{
    QStringList _fileNames = QFileDialog::getOpenFileNames(this, "Open Image Files", QDir::homePath(), "Image Files *.png *.jpg *.jpeg");
    if (!_fileNames.isEmpty())
    {
        clearEverything();

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
    clearRightFrame();
    QThread *imageThread = new QThread;
    ImageWriter *writerWorker = new ImageWriter({
        .images = images,
        .fileNames = fileNames,
        .destinationDirectory = filePathInput->text(),
        .quality = 100 - valueSlider->value(),
        .convertPNG = pngCheckBox->isChecked(),
    });

    writerWorker->moveToThread(imageThread);
    connect(imageThread, &QThread::started, writerWorker, &ImageWriter::start);
    connect(writerWorker, &ImageWriter::finished, this, &MainWindow::writeFinished);
    connect(this, &MainWindow::doneWriting, imageThread, &QThread::quit);
    connect(imageThread, &QThread::finished, writerWorker, &ImageWriter::deleteLater);
    setLoading(true);
    imageThread->start();
}

void MainWindow::writeFinished(qint64 size)
{
    setLoading(false);

    emit doneWriting();
    dataLabel = new QLabel(frameRight);
    dataLabel->setText(
        "<b>Compression Done Successfully</b><br /> Saved to <a href='file://" +
        filePathInput->text() + "'>/tmp</a><br />previous size = " +
        getFileSizeInUnits(initialSize) + "<br /> new size = " +
        getFileSizeInUnits(size) + "<br />Reduced: <b>" +
        getFileSizeInUnits(initialSize - size) + "</b>");

    QPushButton *resultOpenButton = new QPushButton("Open Folder", frameRight);

    connect(resultOpenButton, &QPushButton::clicked, this, [this]()
            { QDesktopServices::openUrl(QUrl::fromLocalFile(filePathInput->text())); });

    rightFrameLayout->addWidget(dataLabel);
    rightFrameLayout->addWidget(resultOpenButton);
}

void MainWindow::InitComponents()
{
    // Left Frame
    frameLeft = new QFrame(this);

    leftScrollArea = new QScrollArea(frameLeft);
    leftScrollAreaWidget = new QWidget(leftScrollArea);

    leftScrollArea->setWidgetResizable(true);

    leftFlowLayout = new FlowLayout(leftScrollAreaWidget);

    leftScrollArea->setWidget(leftScrollAreaWidget);

    // Right Frame
    frameRight = new QFrame(this);

    rightFrameLayout = new QVBoxLayout(frameRight);
    fileSizeLabel = new QLabel(this);
    fileSizeLabel->setAlignment(Qt::AlignCenter);

    // Slider and inputs
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

    controlLayout->addStretch();
    controlLayout->addWidget(label);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(valueSlider);
    controlLayout->addWidget(compressInput);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(pngCheckBox);
    controlLayout->addStretch();

    // Action Buttons;
    actionWidget = new QWidget(this);
    actionLayout = new QHBoxLayout(actionWidget);

    openButton = new QPushButton(actionWidget);
    saveButton = new QPushButton(actionWidget);
    fileDialogButton = new QPushButton(actionWidget);

    openButton->setIcon(QIcon::fromTheme("image"));
    openButton->setText(tr("Open"));
    fileDialogButton->setIcon(QIcon::fromTheme("folder"));
    saveButton->setIcon(QIcon::fromTheme("folder"));
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

void MainWindow::InitSignalSlots()
{
    connect(valueSlider, &QSlider::valueChanged, this, [this](int n)
            { compressInput->setText(QString::number(n)); });
    connect(compressInput, &QLineEdit::textChanged, this, [this](const QString &n)
            { valueSlider->setValue(n.toInt()); });

    connect(openButton, &QPushButton::clicked, this, &MainWindow::on_OpenButtonPressed);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::on_SaveButtonPressed);

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

QString MainWindow::getFileSizeInUnits(const qint64 &size)
{
    const double mb = 1000000.0;
    if (initialSize > mb)
        return QString::number(size / mb) + " MB";
    else
        return QString::number(size * 10 / mb) + " KB";
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

void MainWindow::clearRightFrame()
{
    QLayoutItem *item;
    while ((item = rightFrameLayout->takeAt(0)))
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
    clearLeftFrame();
    clearRightFrame();
}

void MainWindow::setLoading(bool loading)
{
    if (loading)
    {
        openButton->setEnabled(false);
        saveButton->setEnabled(false);
        fileDialogButton->setEnabled(false);

        qDebug() << "Loading...";
        dataLabel = new QLabel(frameRight);
        dataLabel->setAlignment(Qt::AlignCenter);

        dataLabel->setText("Loading...");
        dataLabel->setMovie(loadingGif);
        rightFrameLayout->addWidget(dataLabel, 0, Qt::AlignCenter);
        loadingGif->start();
    }
    else
    {
        openButton->setEnabled(true);
        saveButton->setEnabled(true);
        fileDialogButton->setEnabled(true);

        loadingGif->stop();
        dataLabel->setText("");
        qDebug() << "Not Loading";
        rightFrameLayout->removeWidget(dataLabel);
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

    // 50% width 5% * 10 = 50%
    const int frameWidth = _5Width * 10;
    // 70% height
    const int frameHeight = _5Height * 14;

    frameLeft->setGeometry(_5Width, 0, frameWidth, frameHeight);
    leftScrollArea->setGeometry(0, 0, frameWidth, frameHeight);
    frameRight->setGeometry(width - _5Width * 6 - _5Width, 0, _5Width * 6, frameHeight);

    fileSizeLabel->setGeometry(_5Width, frameHeight, frameWidth, 50);

    controlWidget->setGeometry(0, _5Height * 15, width, 100);
    actionWidget->setGeometry(_5Width, _5Height * 17, width - _5Width * 2, 100);
}
