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
#include <string>



class Controller : public QObject
{
    Q_OBJECT
private:
    QStringList images;
    QString path = "/Users/mehmed/Desktop/uavData";
    QString task = "Raw image preview";
    int currentImageIndex = 0;
public:
    Controller();
    void setTask(QString task);
    void loadImages(QString path);
    QString getDistance(QString fullpath, QString previousFullpath);
    QPixmap cv2Pixmap(cv::Mat mat);
    QString getCurrentImagePath();
    QString getPreviousImagePath();
    int getCurrentImageIndex();
    QStringList getImages();
    void handleInput(QString filename, QString task, int resize);
    void handleChangeDetection(QString filename, int resize);
    int slidePause(int i);

private:
    QString getColorInfo(QString fullpath);
    void updateGuiDetails();
public slots:
    void nextImage();
    void previousImage();
signals:
    void updateDistance(const QString);
    void updateImageDescription(const QString);
    void sendImage(QPixmap img);
};

#endif // CONTROLLER_H
