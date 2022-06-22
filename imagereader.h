#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <QObject>
#include <vector>
#include <QImage>
#include "imageconfig.h"

class ImageReader : public QObject
{
    Q_OBJECT
public:
    ImageReader() = delete;
    ImageReader(QStringList fileNames);

signals:
    void finished(std::vector<ImageConfig>, std::size_t);

public slots:
    void start();
    void quit();

private:
    void ReadImages();

    QStringList fileNames{};
};

#endif // IMAGEREADER_H
