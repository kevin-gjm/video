#ifndef UTIL_H
#define UTIL_H

#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include  <opencv2/imgproc.hpp>
#include <qlogging.h>
#include <QDebug>

class Util
{
public:
    Util();
    static cv::Mat QImage2cvMat(QImage image);
    static QImage cvMat2QImage(const cv::Mat& mat);
};

#endif // UTIL_H
