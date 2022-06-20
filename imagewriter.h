#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <QObject>
#include <QVector>

struct WriteOptions
{
    std::vector<std::vector<uchar>> imageBuffer;
    QStringList fileNames;
    QString destinationDirectory;
    bool convertToJPG;
};
class ImageWriter : public QObject
{
    Q_OBJECT
public:
    ImageWriter(const WriteOptions &_options);
    ImageWriter(WriteOptions &&_options);
    template <typename T>
    constexpr size_t find_from_end(const T &s)
    {
        auto end = s.size() - 1;
        while (1)
        {
            if (s[end--] == '.')
                return static_cast<size_t>(end + 1);
        }
        return 0;
    }

signals:
    void finished();

public slots:
    void start();
    void quit();

private:
    void WriteImages();

    WriteOptions m_options{};
};

#endif // IMAGEWRITER_H
