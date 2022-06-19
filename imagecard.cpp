#include "imagecard.h"
#include "imageviewer.h"

#include <QPixmap>

ImageCard::ImageCard(const QImage &image, const QString &size, QWidget *parent) : QWidget{parent}, imageRef{image}, m_size(size)
{
    InitComponents();
}

ImageCard::ImageCard(QImage &&image, QString &&size, QWidget *parent) : QWidget(parent), imageRef{std::move(image)}, m_size(std::move(size))
{
    InitComponents();
}

void ImageCard::InitComponents()
{
    this->setFixedSize(200, 175);
    cardWidget = new QWidget(this);
    cardLayout = new QVBoxLayout(cardWidget);
    imageLabel = new QLabel("Hello", cardWidget);
    sizeLabel = new QLabel(m_size, this);

    QPixmap pixmap = QPixmap::fromImage(imageRef);
    imageLabel->setFixedSize(150, 75);
    pixmap = pixmap.scaled(imageLabel->width(), imageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);

    sizeLabel->setAlignment(Qt::AlignCenter);

    viewButton = new QPushButton(this);
    viewButton->setIcon(QIcon::fromTheme("view-hidden", QIcon(":view.png")));

    cardLayout->addWidget(imageLabel);
    cardLayout->addWidget(sizeLabel);
    cardLayout->addWidget(viewButton);

    connect(viewButton, &QPushButton::clicked, this, [this]()
            {ImageViewer view = ImageViewer(imageRef); view.exec(); });
}
