#ifndef INPUTREADER_H
#define INPUTREADER_H


#include "QString"
#include "QStringList"
#include "QDir"
#include "imageanalyzer.h"
#include "opencv2/opencv.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


class InputReader
{
    private:
        QString path;
    public:
        InputReader(QString path);
        QStringList getImageList();
        cv::Mat loadImage(QString filename);

};

#endif // INPUTREADER_H
