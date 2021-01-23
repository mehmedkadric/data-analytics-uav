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
#include <QDebug>


using namespace std;

Controller::Controller() {

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
        i = slidePause(i);
        if(i == -1) break;

    }

}

void Controller::handleInput(QString filename, QString task, int resize) {
    InputReader inputReader(filename);
    ImageAnalyzer imageAnalyzer;

    QStringList images = inputReader.getImageList();

    int i = 0;
    double lat1(-1), lon1(-1), lat2(-1), lon2(-1);

    while (i < images.size()) {
        //Read image
        QString imgPath = filename + "/" + images.at(i);
        cv::Mat img = inputReader.loadImage(imgPath);

        Geocoder geocoder;
        JsonParser parser;

        lat1 = geocoder.getLatitude(imgPath);
        lon1 = geocoder.getLongitude(imgPath);
        if(lat2 != -1 && lon2 != -1)
            qDebug() << "Distance: " << geocoder.getDistanceFromLatLongInKm(lat1, lon1, lat2, lon2) * 1000 << " [m]";

        if(!geocoder.checkIfImageIsGeocoded(imgPath))
            geocoder.reverseGeocode(lat1, lon1);

        cv::resize(img, img, cv::Size(img.cols/resize, img.rows/resize));

        QString caption = images.at(i) + " (" + QString::number(i+1) + "/" + QString::number(images.size()) + ")";
        cv::Mat imageSegmented, imageSegmented2, final, final2, final3, showImg;


        //std::unique_ptr<Geocoder> g(new Geocoder());

        if(task == "Segmentation 2D") {

            //cv::fastNlMeansDenoising(img, imageSegmented);
            //cv::imshow(caption.toStdString().c_str(), img);
            imageSegmented = imageAnalyzer.colorSegmentation2D(img);
            cv::imshow(caption.toStdString().c_str(), imageSegmented);

        } else if (task == "Segmentation 2D and Contour detection") {

            cv::imshow(caption.toStdString().c_str(), img);

        } else if (task == "Contour detection") {

            //imageSegmented = imageAnalyzer.colorSegmentation2D(img);
            //imageSegmented2 = imageAnalyzer.geometrySegmentation2D(img, true) - imageAnalyzer.geometrySegmentation2D(img, false);
            imageSegmented2 = imageAnalyzer.geometrySegmentation2D(img, true);
            cv::imshow(caption.toStdString().c_str(), imageSegmented2);

        } else if(task == "Watershed segmentation") {
            //imageSegmented = imageAnalyzer.watershedSegmentation(imageAnalyzer.colorSegmentation2D(img));
            imageSegmented = imageAnalyzer.watershedSegmentation(img);
            cv::imshow(caption.toStdString().c_str(), imageSegmented + img);

        } else if (task == "Raw image preview") {

            cv::imshow(caption.toStdString().c_str(), img);

        }

        lat2 = lat1;
        lon2 = lon1;
        i = slidePause(i);
        if(i == -1) break;
    }
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
