#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "QString"
#include <iostream>
#include "opencv2/opencv.hpp"
#include "QFileDialog"
#include "inputreader.h"
#include "imageanalyzer.h"
#include "changedetector.h"
#include "featurematcher.h"


class Controller
{
public:
    Controller();
    void handleInput(QString filename, QString task, int resize);
    void handleChangeDetection(QString filename, int resize);
    int slidePause(int i);
};

#endif // CONTROLLER_H
