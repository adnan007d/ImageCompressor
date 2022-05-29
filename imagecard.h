#ifndef IMAGECARD_H
#define IMAGECARD_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class ImageCard : public QWidget
{
    Q_OBJECT
public:
    ImageCard(const QImage &image, QWidget *parent = nullptr);

private:
    const QImage &imageRef;

    QLabel *imageLabel = nullptr;
    QPushButton *viewButton = nullptr;
    QWidget *cardWidget = nullptr;
    QVBoxLayout *cardLayout = nullptr;

signals:
};

#endif // IMAGECARD_H
