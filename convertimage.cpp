#include "convertimage.h"
#include <QDebug>
ConvertImage::ConvertImage(const ConvertOptions &options) : m_options(options)
{
}

ConvertImage::ConvertImage(ConvertOptions &&options) : m_options(std::move(options))
{
}

std::vector<int> ConvertImage::getParamVector(std::string_view extension)
{
    if (extension == ".png")
        return {cv::IMWRITE_PNG_COMPRESSION, m_options.quality / 10};

    return {cv::IMWRITE_JPEG_QUALITY, m_options.quality};
}

inline std::string_view ConvertImage::getExtension(std::string_view s, bool convertToJPG)
{
    if (convertToJPG)
        return ".jpg";

    const auto dotIndex = std::find(s.crbegin(), s.crend(), '.');
    return std::string_view((dotIndex + 1).base(), s.end());
}

void ConvertImage::convert()
{
    std::vector<std::vector<uchar>> convertedImages{};

    convertedImages.reserve(m_options.imageMats.size());
    const auto &imageMats = m_options.imageMats;
    const auto &fileNames = m_options.fileNames;

    int i = 0;
    for (const auto &imageMat : imageMats)
    {
        std::vector<uchar> buffer{};
        const std::string fileName = fileNames[i].toStdString();

        const std::string_view extension{getExtension(fileName, m_options.convertPNG)};

        std::vector<int> params{getParamVector(extension)};

        cv::imencode(extension.data(), imageMat, buffer, params);
        convertedImages.push_back(std::move(buffer));
        ++i;
    }

    emit finished(convertedImages);
}

void ConvertImage::start()
{
    convert();
}

void ConvertImage::quit()
{
    // emit
}
