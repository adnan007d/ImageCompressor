#include "imagecard.h"
#include "imageviewer.h"

#include <QPixmap>

ImageCard::ImageCard(const QImage &image, QWidget *parent) : QWidget{parent}, imageRef{image}
{
    InitComponents();
}

ImageCard::ImageCard(QImage &&image, QWidget *parent) : QWidget(parent), imageRef{std::move(image)}
{
    InitComponents();
}

void ImageCard::InitComponents()
{
    this->setFixedSize(150, 125);
    cardWidget = new QWidget(this);
    cardLayout = new QVBoxLayout(cardWidget);
    imageLabel = new QLabel("Hello", cardWidget);

    QPixmap pixmap = QPixmap::fromImage(imageRef);
    imageLabel->setFixedSize(150, 75);
    pixmap = pixmap.scaled(imageLabel->width(), imageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);

    viewButton = new QPushButton(this);
    viewButton->setIcon(QIcon::fromTheme("view-hidden", QIcon(":view.png")));

    cardLayout->addWidget(imageLabel);
    cardLayout->addWidget(viewButton);

    connect(viewButton, &QPushButton::clicked, this, [this]()
            {ImageViewer view = ImageViewer(imageRef); view.exec(); });
}
