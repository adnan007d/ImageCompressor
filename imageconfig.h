#ifndef __ImageConfig__
#define __ImageConfig__

#include <opencv2/imgcodecs.hpp>

struct ImageConfig
{
    cv::Mat imageMat;
    qint64 size;
};

struct ConvertOptions
{
    std::vector<cv::Mat> imageMats;
    QStringList fileNames;
    int quality;
    bool convertPNG;
};

template <typename T>
constexpr size_t find_dot_from_end(const T &s)
{
    auto end = s.size() - 1;
    while (end >= 0)
    {
        if (s[end--] == '.')
            return static_cast<size_t>(end + 1);
    }
    return -1;
}

#endif // __ImageConfig__
