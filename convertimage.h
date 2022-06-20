#ifndef __CONVERT_IMAGE__
#define __CONVERT_IMAGE__

#include <QObject>
#include "imageconfig.h"

class ConvertImage : public QObject
{
    Q_OBJECT
public:
    ConvertImage(const ConvertOptions &options);
    ConvertImage(ConvertOptions &&options);

signals:
    void finished(std::vector<std::vector<uchar>>);

public slots:
    void start();
    void quit();
    std::string_view getExtension(std::string_view path, bool convertToJPG = false);

private:
    void convert();
    std::vector<int> getParamVector(std::string_view extension);
    ConvertOptions m_options{};
};

#endif // __CONVERT_IMAGE__
