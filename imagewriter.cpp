#include "imagewriter.h"

#include <QFileInfo>
#include <QImageWriter>

ImageWriter::ImageWriter(const WriteOptions &_options) : m_options{_options}
{
}

ImageWriter::ImageWriter(WriteOptions &&_options) : m_options{std::move(_options)}
{
}

QString getFileName(const QString &fileName, bool convertToJPG)
{
    QFileInfo f = QFileInfo(fileName);

    QString baseName = f.baseName();

    if (convertToJPG && (f.suffix() == ".png"))
        return baseName + ".jpg";

    return f.fileName();
}

void ImageWriter::WriteImages()
{
    const auto &imageBuffers = m_options.imageBuffer;
    const auto &fileNames = m_options.fileNames;
    const auto &destination = m_options.destinationDirectory;

    int i = 0;
    for (const auto &imageBuffer : imageBuffers)
    {
        const QString fileName = getFileName(fileNames[i], m_options.convertToJPG);
        QFile fout(destination + "/Compressed" + fileName);
        fout.open(QIODevice::WriteOnly);
        fout.write(reinterpret_cast<const char *>(imageBuffer.data()), imageBuffer.size());
        fout.close();
        ++i;
    }
    emit finished();
}

void ImageWriter::start()
{

    WriteImages();
}

void ImageWriter::quit()
{
    emit finished();
}
