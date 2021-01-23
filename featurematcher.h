#ifndef FEATUREMATCHER_H
#define FEATUREMATCHER_H


#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"


class FeatureMatcher
{
public:
    FeatureMatcher();
    cv::Mat findFeatures(cv::Mat im1, cv::Mat im2);
    cv::Mat matchFeatures(cv::Mat im1, cv::Mat im2);

};

#endif // FEATUREMATCHER_H
