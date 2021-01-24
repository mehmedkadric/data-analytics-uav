#ifndef IMAGEANALYZER_H
#define IMAGEANALYZER_H


#include "opencv2/opencv.hpp"
#include "QString"
#include "map"
#include "iterator"
#include "QFileDialog"
#include "inputreader.h"
#include "controller.h"
#include <QMap>


class ImageAnalyzer
{
private:
    QMap<QString, int> colorFrequency;
    std::map<QString, cv::Scalar> generateHsvRanges();
    cv::Scalar getColor(QString colorLabel);
    std::vector<cv::Scalar> getMinColorRanges(std::map<QString, cv::Scalar> hsvRanges);
    std::vector<cv::Scalar> getMaxColorRanges(std::map<QString, cv::Scalar> hsvRanges);
    std::vector<QString> getMinColorLabels(std::map<QString, cv::Scalar> hsvRanges);
    std::vector<QString> getMaxColorLabels(std::map<QString, cv::Scalar> hsvRanges);

public:
    ImageAnalyzer();
    QMap<QString, int> getColorFrequency();
    QMap<QString, int> calculateColorHistogram(QString fullpath);
    cv::Mat emphasizeObjects2D(cv::Mat img);
    cv::Mat colorSegmentation2D(cv::Mat img);
    cv::Mat geometrySegmentation2D(cv::Mat img, bool filled);
    cv::Mat watershedSegmentation(cv::Mat src);
    cv::Mat homogenizeColors(cv::Mat img, cv::Mat mask);
    int calculateMaxHistogramValue(cv::Mat img, bool display);

    void size(cv::Mat img);
};

#endif // IMAGEANALYZER_H
