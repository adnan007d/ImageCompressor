#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <QObject>
#include <QVector>
#include <QImage>

class ImageReader : public QObject
{
    Q_OBJECT
public:
    ImageReader() = delete;
    ImageReader(QStringList fileNames);

signals:
    void finished(QVector<QImage>, qint64);

public slots:
    void start();
    void quit();

private:
    void ReadImages();

    QStringList fileNames{};
};

#endif // IMAGEREADER_H
