#include "imagewriter.h"

#include <QFileInfo>
#include <QImageWriter>

ImageWriter::ImageWriter(CompressOptions _options) : options{std::move(_options)}
{
}

void ImageWriter::WriteImages()
{
    qint64 size = 0;
    QFileInfo f = QFileInfo();

    QVector<QImage> &images = options.images;
    QStringList &fileNames = options.fileNames;
    QString &destination = options.destinationDirectory;

    for (int i = 0; i < images.size(); ++i)
    {
        f.setFile(fileNames[i]);
        const QString destinationPath = destination + "/COMPRESSED" + f.fileName();
        f.setFile(destinationPath);
        QImageWriter writer = QImageWriter(destinationPath);
        writer.setQuality(options.quality);
        writer.write(images[i].convertedTo(QImage::Format::Format_RGB16));
        size += f.size();
    }
    emit finished(size);
}

void ImageWriter::WriteImagesConvertPng()
{
    qint64 size = 0;
    QFileInfo f = QFileInfo();

    QVector<QImage> &images = options.images;
    QStringList &fileNames = options.fileNames;
    QString &destination = options.destinationDirectory;

    for (int i = 0; i < images.size(); ++i)
    {
        f.setFile(fileNames[i]);
        const auto fileName = f.fileName();
        const int dotIndex = find_from_end(fileName);
        const QString destinationPath = destination + "/COMPRESSED" + fileName.left(dotIndex) + ".jpg";
        f.setFile(destinationPath);
        QImageWriter writer = QImageWriter(destinationPath);
        writer.setQuality(options.quality);
        writer.write(images[i].convertedTo(QImage::Format::Format_RGB16));
        size += f.size();
    }
    emit finished(size);
}

void ImageWriter::start()
{
    if(options.convertPNG)
        WriteImagesConvertPng();
    else
        WriteImages();
}

void ImageWriter::quit()
{
    emit finished({});
}
