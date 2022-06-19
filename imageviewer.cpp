#include "imageviewer.h"

ImageViewer::ImageViewer(const QImage &image) : imageRef{image}
{

    imageLabel = new QLabel(this);
    imageLabel->setGeometry(0, 0, width(), height());

    imageLabel->setAlignment(Qt::AlignCenter);
}

inline QPixmap ImageViewer::getScaledPixmap()
{
    return QPixmap::fromImage(imageRef).scaled(imageLabel->width(), imageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ImageViewer::resizeEvent([[maybe_unused]] QResizeEvent *event)
{
    const int width = this->width();
    const int height = this->height();

    imageLabel->setGeometry(0, 0, width, height);
    imageLabel->setPixmap(getScaledPixmap());
    imageLabel->update();
}
