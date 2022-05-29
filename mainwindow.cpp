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
    images = qMove(_images);
    initialSize = qMove(size);

    for (const auto &image : images)
    {
        ImageCard *card = new ImageCard(image, frameLeft);
        leftFlowLayout->addWidget(card);
    }
    fileSizeLabel->setText(getFileSizeInUnits(initialSize));
    setLoading(false);
}

void MainWindow::on_OpenButtonPressed()
{
    fileNames = QFileDialog::getOpenFileNames(this, "Open Image Files", QDir::homePath(), "Image Files *.png *.jpg *.jpeg");
    if (!fileNames.isEmpty())
    {
        QThread *imageThread = new QThread(this);
        ImageReader *readerWorker = new ImageReader(fileNames);

        readerWorker->moveToThread(imageThread);

        connect(imageThread, &QThread::started, readerWorker, &ImageReader::start);
        connect(readerWorker, &ImageReader::finished, this, &MainWindow::renderImageCards);
        connect(imageThread, &QThread::finished, readerWorker, &ImageReader::deleteLater);
        setLoading(true);
        imageThread->start();
    }
}

void MainWindow::on_SaveButtonPressed()
{
    QThread *imageThread = new QThread(this);
    ImageWriter *writerWorker = new ImageWriter({
        .images = images,
        .fileNames = fileNames,
        .destinationDirectory = filePathInput->text(),
        .quality = 100 - valueSlider->value(),
        .convertPNG = false,
    });

    writerWorker->moveToThread(imageThread);
    connect(imageThread, &QThread::started, writerWorker, &ImageWriter::start);
    connect(writerWorker, &ImageWriter::finished, this, &MainWindow::writeFinished);
    connect(imageThread, &QThread::finished, writerWorker, &ImageWriter::deleteLater);
    setLoading(true);
    imageThread->start();
}

void MainWindow::writeFinished(qint64 size)
{
    setLoading(false);

    QMessageBox m = QMessageBox(this);
    m.setIcon(QMessageBox::Information);
    m.setWindowTitle("File Compressed Successfully");
    m.setText("Files saved to <b>" + filePathInput->text() + "<b>\n New Size" + getFileSizeInUnits(size));
    m.exec();
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

    controlLayout->addStretch();
    controlLayout->addWidget(label);
    controlLayout->addSpacing(20);
    controlLayout->addWidget(valueSlider);
    controlLayout->addWidget(compressInput);
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
                    filePathInput->setText(qMove(path));
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

void MainWindow::setLoading(bool loading)
{
    if (loading)
    {
        qDebug() << "Loading...";
        dataLabel = new QLabel(this);
        dataLabel->setAlignment(Qt::AlignCenter);

        dataLabel->setText("Loading...");
        dataLabel->setMovie(loadingGif);
        rightFrameLayout->addWidget(dataLabel, 0, Qt::AlignCenter);
        loadingGif->start();
    }
    else
    {
        loadingGif->stop();
        dataLabel->setText("");
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
