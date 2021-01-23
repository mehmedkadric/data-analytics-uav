#ifndef CHANGEDETECTOR_H
#define CHANGEDETECTOR_H

#include "QString"
#include "inputreader.h"
#include "imageanalyzer.h"
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/features2d.hpp"


class ChangeDetector
{
public:
    ChangeDetector();
    cv::Mat findDifferences(cv::Mat img1, cv::Mat img2);
};

#endif // CHANGEDETECTOR_H
