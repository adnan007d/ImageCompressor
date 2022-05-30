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
    template<typename T>
    constexpr size_t find_from_end(const T &s)
    {
        auto end = s.size() - 1;
        while(1)
        {
            if(s[end--] == '.')
                return static_cast<size_t>(end+1);
        }
        return 0;
    }

signals:
    void finished(qint64);

public slots:
    void start();
    void quit();

private:
    void WriteImages();
    void WriteImagesConvertPng();

    CompressOptions options{};
};

#endif // IMAGEWRITER_H
