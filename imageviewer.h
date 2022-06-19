#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QDialog>
#include <QLabel>

class ImageViewer : public QDialog
{
public:
    ImageViewer(const QImage &image);

private:
    const QImage &imageRef;
    QLabel *imageLabel = nullptr;

    void resizeEvent(QResizeEvent *event);
    QPixmap getScaledPixmap();
};

#endif // IMAGEVIEWER_H
