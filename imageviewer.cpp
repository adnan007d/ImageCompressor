#include "imageviewer.h"

ImageViewer::ImageViewer(const QImage &image) : imageRef{image}
{

    imageLabel = new QLabel(this);
    imageLabel->setGeometry(0, 0, width(), height());

    QPixmap pixmap = QPixmap();
    pixmap = pixmap.fromImage(imageRef);
    pixmap = pixmap.scaled(imageLabel->width(), imageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
}

void ImageViewer::resizeEvent([[maybe_unused]] QResizeEvent *event)
{
    const int width = this->width();
    const int height = this->height();

    imageLabel->setGeometry(0, 0, width, height);
    imageLabel->setPixmap(QPixmap::fromImage(imageRef).scaled(imageLabel->width(), imageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->update();
}
