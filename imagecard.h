#ifndef IMAGECARD_H
#define IMAGECARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QImage>

class ImageCard : public QWidget
{
    Q_OBJECT
public:
    ImageCard(const QImage &image, const QString &size, QWidget *parent = nullptr);
    ImageCard(QImage &&image, QString &&size, QWidget *parent = nullptr);

    void InitComponents();

private:
    QImage imageRef{};
    QString m_size{};

    QLabel *imageLabel = nullptr;
    QLabel *sizeLabel = nullptr;
    QPushButton *viewButton = nullptr;
    QWidget *cardWidget = nullptr;
    QVBoxLayout *cardLayout = nullptr;

signals:
};

#endif // IMAGECARD_H
