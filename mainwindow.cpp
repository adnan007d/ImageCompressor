#include "imagecard.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QIntValidator>
#include <QMessageBox>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //        QImageReader reader = QImageReader("/home/barty/Desktop/1.png");
    //        QImage img = reader.read();

    //        img.convertTo(QImage::Format::Format_RGB16);
    //        qDebug() << img;

    //        QImageWriter writer = QImageWriter("new.jpg");
    //        writer.setQuality(20);
    //        writer.write(img);

    this->setMinimumWidth(850);


    InitComponents();
    InitSignalSlots();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitComponents()
{
    // Both Frames
    frameLeft = new QFrame(this);

    //    frameLeft->setStyleSheet("background-color:white;");


    leftScrollArea = new QScrollArea(frameLeft);
    leftScrollAreaWidget = new QWidget(leftScrollArea);

    leftScrollArea->setWidgetResizable(true);

    leftFlowLayout = new FlowLayout(leftScrollAreaWidget);

    leftScrollArea->setWidget(leftScrollAreaWidget);

    // Right Frame
    frameRight = new QFrame(this);
//    frameRight->setStyleSheet("background-color:white;");

    rightFrameLayout = new QVBoxLayout(frameRight);
    dataLabel = new QLabel(this);
    rightFrameLayout->addWidget(dataLabel, 0, Qt::AlignCenter);


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

    // Opening Files
    connect(openButton, &QPushButton::clicked, this,
    [this]()
    {
        fileNames = QFileDialog::getOpenFileNames(this, "Open Image Files", QDir::homePath(), "Image Files *.png *.jpg *.jpeg");
        if(!fileNames.isEmpty())
        {
            renderCards(fileNames);
        }
        else
        {
            // TODO: Error Handling

        }
    });

    // Saving Files
    connect(saveButton, &QPushButton::clicked, this,
    [this]()
    {

        QFileInfo f = QFileInfo();
        setLoading(true);
        for(int i = 0; i < images.size(); ++i)
        {

            f.setFile(fileNames[i]);
            const QString destinationPath = filePathInput->text() + "/COMPRESSED" +  f.fileName();

            QImageWriter writer = QImageWriter(destinationPath);
            QImage _image = images[i].convertedTo(QImage::Format::Format_RGB16);

            writer.setQuality(100 - valueSlider->value());
            writer.write(_image);
            qDebug() << "Done One";
        }
        setLoading(false);

        QMessageBox m = QMessageBox(this);
        m.setIcon(QMessageBox::Information);
        m.setWindowTitle("File Compressed Successfully");
        m.setText("Files saved to " + filePathInput->text());
        m.exec();

    });

    // Selecting Destination Directory
    connect(fileDialogButton, &QPushButton::clicked, this,
    [this]()
    {
     QString path = QFileDialog::getExistingDirectory(this, "Select Destination Folder", filePathInput->text());
     if(!path.isNull())
     {
         filePathInput->setText(qMove(path));
     }
    });
}

void MainWindow::renderCards(const QStringList &fileNames)
{
    images.reserve(fileNames.size());
    QFileInfo f = QFileInfo();
    setLoading(true);
    for (const auto &fileName : fileNames)
    {
        f.setFile(fileName);
        initialSize += f.size();
        QImageReader reader = QImageReader(fileName);
        QImage img = reader.read();

        images.push_back(img);
        ImageCard *card = new ImageCard(qMove(img), frameLeft);
        leftFlowLayout->addWidget(card);
    }
    setLoading(false);
    qDebug() << static_cast<double>(initialSize) / 1000000 << "MB";

}

void MainWindow::setLoading(bool loading)
{
    if(loading)
    {
        qDebug() << "Loading...";
        dataLabel->setText("Loading...");
    }
    else
    {
        dataLabel->setText("");
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

    //        qDebug() << width << ' ' << height << ' ' << _5Width << ' ' << _5Height << ' ' << _5Width * 8 << ' ' << _5Height * 16 << '\n';
    frameLeft->setGeometry(_5Width, 0, frameWidth, frameHeight);
    leftScrollArea->setGeometry(0, 0, frameWidth, frameHeight);
    frameRight->setGeometry(width - _5Width * 6 - _5Width, 0, _5Width * 6, frameHeight);
//    rightScrollArea->setGeometry(0, 0, frameWidth, frameHeight);

    controlWidget->setGeometry(0, _5Height * 15, width, 100);
    actionWidget->setGeometry(_5Width, _5Height * 17, width - _5Width * 2, 100);
    //    valueSlider->setGeometry(width / 2 - 100, _5Height * 15, 200, valueSlider->height());
}
