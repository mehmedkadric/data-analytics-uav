#include "inputreader.h"


InputReader::InputReader(QString path)
{
    this->path = path;
}

QStringList InputReader::getImageList(){
     QDir directory(this->path);
     QStringList images = directory.entryList(QStringList() << "*.png" << "*.PNG" << "*.jpg" << "*.JPG",QDir::Files);
     return images;
}

cv::Mat InputReader::loadImage(QString filename){
    cv::Mat img;
    try {
        img = cv::imread(filename.toUtf8().data());
        return img;
    } catch (std::exception e) {
        std::cout<<e.what()<<std::endl;
    }
    return cv::Mat::zeros(cv::Size(1, 1), CV_64FC1);
}

