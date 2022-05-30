#include "imagereader.h"

#include <QFileInfo>
#include <QImageReader>

ImageReader::ImageReader(QStringList _fileNames) : fileNames{std::move(_fileNames)}
{
}

void ImageReader::ReadImages()
{
    QVector<QImage> images{};
    images.reserve(fileNames.size());
    QFileInfo f;
    qint64 size = 0;
    for (const auto &fileName : fileNames)
    {
        f.setFile(fileName);
        size += f.size();
        QImageReader reader = QImageReader(fileName);
        images.push_back(reader.read());
    }
    emit finished(images, size);
}

void ImageReader::start()
{
    ReadImages();
}

void ImageReader::quit()
{
    emit finished({}, {});
}
