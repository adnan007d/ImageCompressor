#ifndef __ImageConfig__
#define __ImageConfig__

#include <opencv2/imgcodecs.hpp>

struct ImageConfig
{
    cv::Mat imageMat;
    qint64 size;
};


#endif // __ImageConfig__
