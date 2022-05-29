#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <QObject>
#include <QVector>
#include <QImage>

struct CompressOptions
{
    QVector<QImage> images;
    QStringList fileNames;
    QString destinationDirectory;
    int quality;
    bool convertPNG;
};
class ImageWriter : public QObject
{
    Q_OBJECT
public:
    ImageWriter(CompressOptions _options);

signals:
    void finished(qint64);

public slots:
    void start();
    void quit();

private:
    void WriteImages();

    CompressOptions options{};
};

#endif // IMAGEWRITER_H
