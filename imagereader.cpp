#include "imagereader.h"

#include <QFileInfo>
#include <opencv2/imgproc.hpp>
#include <QImageReader>

ImageReader::ImageReader(QStringList _fileNames) : fileNames{std::move(_fileNames)}
{
}

void ImageReader::ReadImages()
{
    std::vector<ImageConfig> images{};
    images.reserve(fileNames.size());
    QFileInfo f;
    qint64 size = 0;
    for (const auto &fileName : fileNames)
    {
        f.setFile(fileName);
        size += f.size();
        images.push_back({cv::imread(fileName.toStdString()), f.size()});
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
