#include "controller.h"
#include <iostream>
#include <stdio.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <string>
#include <QDebug>
#include "exif.h"
#include "geocoder.h"
#include "jsonparser.h"
#include <QPixmap>
#include <ui_mainwindow.h>
#include "mainwindow.h"
#include <cmath>
#include <algorithm>


using namespace std;

Controller::Controller() {

}

void Controller::setTask(QString task) {
    this->task = task;
}

void Controller::loadImages(QString path) {
    InputReader inputReader(path);
    this->path = path;
    this->images = inputReader.getImageList();
    emit sendImage(QPixmap(path + "/" + this->images.at(0)));
}

void Controller::nextImage() {
    currentImageIndex++;
    if(currentImageIndex > images.size() - 1)
        currentImageIndex = images.size() - 1;
    if(currentImageIndex < 0)
        currentImageIndex = 0;
    updateGuiDetails();
}

void Controller::previousImage() {
    currentImageIndex--;
    if(currentImageIndex < 0)
        currentImageIndex = 0;
    updateGuiDetails();
}

int Controller::getCurrentImageIndex() {
    return this->currentImageIndex;
}

QStringList Controller::getImages() {
    return this->images;
}

QString Controller::getCurrentImagePath() {
         return path + "/" + images.at(currentImageIndex);
}

QString Controller::getPreviousImagePath() {
    if(currentImageIndex >= 1)
         return path + "/" + images.at(currentImageIndex - 1);
    else
        return path + "/" + images.at(0);
}

void Controller::updateGuiDetails() {
    if(currentImageIndex == 0)
        loadImages(this->path);

    QString fullpath = path + "/" + images.at(currentImageIndex);
    QString previousFullpath = getPreviousImagePath();

    ImageAnalyzer imageAnalyzer;
    InputReader reader(path);

    cv::Mat img = reader.loadImage(fullpath);
    cv::Mat imageSegmented;

    QString msg = getDistance(fullpath, previousFullpath);
    emit updateDistance(msg);
    QString imgDetails = "";
    imgDetails += getColorInfo(fullpath);
    emit updateImageDescription(imgDetails);

    if(task == "Segmentation 2D") {
        imageSegmented = imageAnalyzer.emphasizeObjects2D(img) + img;
        //imageSegmented = imageAnalyzer.colorSegmentation2D(img);
        //imageSegmented = imageAnalyzer.colorSegmentation2D(img) + img;
    } else if (task == "Segmentation 2D and Contour detection") {
        imageSegmented = img;
    } else if (task == "Contour detection") {
        imageSegmented = imageAnalyzer.geometrySegmentation2D(img, true);
    } else if(task == "Watershed segmentation") {
        imageSegmented = imageAnalyzer.watershedSegmentation(img);
    } else if (task == "Raw image preview") {
        imageSegmented = img;
    }

    QPixmap pixmap = cv2Pixmap(imageSegmented);
    //QPixmap pixmap(fullpath);
    emit sendImage(pixmap);
}


QString Controller::getColorInfo(QString fullpath) {
    ImageAnalyzer imageAnalyzer;
    QString imgDetails = "MOST FREQUENT COLORS (>5%)\n";
    QMap<QString, int> colorHistogram = imageAnalyzer.calculateColorHistogram(fullpath);
    std::vector<std::pair<QString, int>> colorHistogramSorted;
    for (auto it=colorHistogram.begin(); it!=colorHistogram.end(); it++) {
      colorHistogramSorted.push_back(std::make_pair(it.key(), it.value()));
    }

    sort(colorHistogramSorted.begin(), colorHistogramSorted.end(), [](const std::pair<QString, int> &a, const std::pair<QString, int> &b){return (a.second > b.second);});

    if(colorHistogramSorted.size() > 0) {
        float sum = 0;
        for( int i = 0; i < static_cast<int>(colorHistogramSorted.size()); i++ )
            sum += colorHistogramSorted[i].second;

        for( int i = 0; i < static_cast<int>(colorHistogramSorted.size()); i++ ) {
            const auto& value = colorHistogramSorted[i].second;
            float perc = static_cast<int>((((value)*100.0/sum)*10))/10.0;
            if (perc > 5)
                imgDetails +=  QString::number(i + 1) + ". " + colorHistogramSorted[i].first + " (" +  QString::number(perc) + "%)\n";
        }
    }

    return imgDetails;
}


QPixmap Controller::cv2Pixmap(cv::Mat matOrig) {
    cv::Mat mat;
    cv::cvtColor(matOrig, mat, cv::COLOR_BGR2RGB);
    return QPixmap::fromImage(QImage((unsigned char*) mat.data, mat.cols, mat.rows, QImage::Format_RGB888));
}

QString Controller::getDistance(QString fullpath, QString previousFullpath) {
    double lat1, lon1, lat2, lon2;
    Geocoder geo;
    lat1 = geo.getLatitude(fullpath);
    lon1 = geo.getLongitude(fullpath);
    lat2 = geo.getLatitude(previousFullpath);
    lon2 = geo.getLongitude(previousFullpath);

    QString msg = "Distance from the previous image: ";
    msg += QString::number((int)((geo.getDistanceFromLatLongInKm(lat1, lon1, lat2, lon2)*1000)*100.0)/100.0);
    msg += " [m]";

    if(lat1 == -1 || lon1 == -1 || lat2 == -1 || lon2 == -1)
        msg = "Unable to calculate distance from the previous image.";

    return msg;
}

void Controller::handleChangeDetection(QString filename, int resize) {
    ChangeDetector changeDetector;
    FeatureMatcher featureMatcher;
    InputReader inputReader(filename);
    QStringList images = inputReader.getImageList();

    if(images.size() <= 2) return;

    for (int i = 1; i < images.size(); i += 0) {
        cv::Mat img1 = inputReader.loadImage(filename + "/" + images.at(i-1));
        cv::Mat img2 = inputReader.loadImage(filename + "/" + images.at(i));

        cv::resize(img1, img1, cv::Size(img1.cols/resize, img1.rows/resize));
        cv::resize(img2, img2, cv::Size(img2.cols/resize, img2.rows/resize));


        cv::Mat img2Projected = featureMatcher.findFeatures(img1, img2);
        cv::Mat diff = changeDetector.findDifferences(img2Projected, img1);
        //cv::Mat diff;
        //cv::absdiff(img1, img2Projected, diff);

        //cv::Mat diff2 = changeDetector.findDifferences(img2, img1);
        //cv::Mat img2Projected = featureMatcher.findFeatures(img1, img2);

        //cv::absdiff(img1, img2Projected, diff);

        QString caption = images.at(i) + " (" + QString::number(i) + "/" + QString::number(images.size()-1) + ")";
        cv::imshow(caption.toStdString().c_str(), diff + img1);
        //i = slidePause(i);
        i = slidePause(i);
        if(i == -1) break;

    }

}

void Controller::handleInput(QString filename, QString task, int resize) {

}

int Controller::slidePause(int i) {

    int key = cv::waitKey(0);

    if(key == 3) {
        //Right arrow pressed
        cv::destroyAllWindows();
        i++;
    } else if (key == 2) {
        //Left arrow pressed
        cv::destroyAllWindows();
        i--;
        if(i < 0) i = 0;
    } else if (key == 27) {
        //Esc key pressed
        cv::destroyAllWindows();
        i = -1;
    }

    return i;
}
