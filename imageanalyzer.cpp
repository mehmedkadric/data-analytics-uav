#include "imageanalyzer.h"
#include <QDebug>


ImageAnalyzer::ImageAnalyzer()
{

}


cv::Mat ImageAnalyzer::watershedSegmentation(cv::Mat orig) {
    cv::Mat src;
    cv::GaussianBlur(orig, src, cv::Size(15, 15), 0);
    //cv::GaussianBlur(orig, src, cv::Size(0, 0), 3);
    //cv::addWeighted(orig, 1.5, src, -0.5, 0, src);

    // Create binary image from source image
    cv::Mat bw;
    cv::cvtColor(src, bw, cv::COLOR_BGR2GRAY);
    cv::threshold(bw, bw, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Perform the distance transform algorithm
    cv::Mat dist;
    cv::distanceTransform(bw, dist, cv::DIST_L2, 3);

    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    cv::normalize(dist, dist, 0, 10., cv::NORM_MINMAX);
    //cv::imshow("dist", dist);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    cv::threshold(dist, dist, .1, 10., cv::THRESH_BINARY);
    //cv::imshow("dist2", dist);

    // Create the CV_8U version of the distance image
    // It is needed for cv::findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);

    // Find total markers
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(dist_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    int ncomp = contours.size();

    // Create the marker image for the watershed algorithm
    cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32SC1);

    // Draw the foreground markers
    for (int i = 0; i < ncomp; i++)
        cv::drawContours(markers, contours, i, cv::Scalar::all(i+1), -1);

    // Draw the background marker
    cv::circle(markers, cv::Point(5,5), 3, CV_RGB(255,255,255), -1);
    //cv::imshow("markers", markers*10000);

    // Perform the watershed algorithm
    cv::watershed(src, markers);

    // Generate random colors
    std::vector<cv::Vec3b> colors;
    for (int i = 0; i < ncomp; i++)
    {
        int b = cv::theRNG().uniform(0, 255);
        int g = cv::theRNG().uniform(0, 255);
        int r = cv::theRNG().uniform(0, 255);

        colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    // Create the result image
    cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);

    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= ncomp)
                dst.at<cv::Vec3b>(i,j) = colors[index-1];
            else
                dst.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
        }
    }
    return dst;
}


cv::Mat ImageAnalyzer::geometrySegmentation2D(cv::Mat img, bool filled) {

    cv::Mat imgSegmented, imgGray, _imgGray, _img2;

    cv::GaussianBlur(img, _img2, cv::Size(0, 0), 3);
    cv::addWeighted(_img2, 1, img, 0.5, 2, img);
    //return img;
    img.copyTo(imgSegmented);

    cv::cvtColor(imgSegmented, imgGray, cv::COLOR_BGR2GRAY);

    //cv::GaussianBlur(_imgGray, _imgGray, cv::Size(7, 7), 0);
    //cv::bilateralFilter ( _imgGray, imgGray, 9, 9, 7 );
    //cv::GaussianBlur(_imgGray, imgGray, cv::Size(15, 15), 0);

    cv::Mat canny_output, canny_output2, orig_img, _img, out_img;
    orig_img = imgGray;

    cv::threshold(orig_img, out_img, 10, 255, 0);
    cv::Mat element = cv::getStructuringElement(0, cv::Size(5, 5));
    //cv::morphologyEx(out_img, out_img, 2, element);
    //cv::morphologyEx(out_img, out_img, 3, element);

    double otsu_thresh_val = cv::threshold(
                orig_img, _img, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU
                );

    double high_thresh_val  = otsu_thresh_val,
            lower_thresh_val = otsu_thresh_val * 0.5;

    cv::Canny( orig_img, canny_output, lower_thresh_val, high_thresh_val );

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours( canny_output, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE );

    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    std::vector<cv::Rect> boundRect( contours.size() );

    for( size_t i = 0; i < contours.size(); i++ ) {
        cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( cv::Mat(contours_poly[i]) );
        //std::cout<<"Countour area: " << boundRect[i].height * boundRect[i].width << std::endl;
        //std::cout<<"Countour area: " << cv::contourArea(contours_poly[i]) << std::endl;
    }

    cv::Mat drawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 );

    for( size_t i = 0; i< contours.size(); i++ ) {
        int bbArea = boundRect[i].height * boundRect[i].width;
        double bbAreaPercentage = bbArea * 100.0 / (canny_output.rows * canny_output.cols);
        if(cv::contourArea(contours[i]) > 25) {
            if(filled)
                cv::drawContours( drawing, contours, (int)i, cv::Scalar(0, 0, 255), cv::FILLED, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
            else
                cv::drawContours( drawing, contours, (int)i, cv::Scalar(0, 0, 255), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
            //cv::rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 0, 255), 5, 8, 0 );
            //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
        }
    }


    //cv::morphologyEx(drawing, drawing, 2, element);
    //cv::morphologyEx(drawing, drawing, 3, element);
    return drawing;

}


cv::Mat ImageAnalyzer::homogenizeColors(cv::Mat img, cv::Mat mask) {
    cv::Mat imgGray, out_image, mask2;
    cv::bitwise_and(img, img, out_image, mask);

    cv::threshold(out_image, mask2, 10, 255, 0);

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    int cnt = 0;
    cv::Mat finalImage(mask.rows, mask.cols, CV_8UC3);
    for(int i = 0; i < contours.size(); i++) {
        if(cv::contourArea(contours[i]) > 2000) {
            cv::Mat drawing = cv::Mat::zeros(out_image.size(), CV_8UC1);
            cv::Mat drawing2 = cv::Mat::zeros(out_image.size(), CV_8UC3);
            cv::Mat singleContour;
            cv::drawContours( drawing, contours, (int) i, cv::Scalar(255), cv::FILLED);
            cv::bitwise_and(img, img, singleContour, drawing);


            if(calculateMaxHistogramValue(singleContour, false) == 14) {
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(0, 75, 150)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            } else if(calculateMaxHistogramValue(singleContour, false) == 29) {
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(0, 255, 255)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            } else if(calculateMaxHistogramValue(singleContour, false) == 59) {
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(0, 255, 0)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            } else if(calculateMaxHistogramValue(singleContour, false) == 119) {
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(255, 0, 0)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            }  else if(calculateMaxHistogramValue(singleContour, false) == 140) {
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(211, 0, 148)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            } else if (calculateMaxHistogramValue(singleContour, false) == 173) {
                // std::cout<<calculateMaxHistogramValue(singleContour)<<std::endl;
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(0, 0, 255)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            } else {
                cv::Mat colorMask(drawing.rows, drawing.cols, CV_8UC3, cv::Scalar(50, 50, 50)), final;
                cv::bitwise_and(colorMask, colorMask, final, drawing);
                finalImage += final;
            }
        }
    }

    return finalImage;
}

cv::Mat ImageAnalyzer::emphasizeObjects2D(cv::Mat img) {
    cv::Mat imgSegmented, _image;
    //img.copyTo(_image);
    //cv::GaussianBlur(imgSegmented, imgSegmented, cv::Size(15, 15), 0);

    /*
    cv::bilateralFilter ( _image, imgSegmented, 9, 9, 7 );
    cv::bilateralFilter ( imgSegmented, _image, 9, 9, 7 );
    cv::bilateralFilter ( _image, imgSegmented,  9, 9, 7 );
    cv::bilateralFilter ( imgSegmented, _image, 9, 9, 7 );
    cv::bilateralFilter ( _image, imgSegmented,  9, 9, 7 );
    */

    cv::cvtColor(img, imgSegmented, cv::COLOR_BGR2HSV);

    std::map<QString, cv::Scalar> hsvRanges = generateHsvRanges();

    std::vector<cv::Scalar> minColorRanges = getMinColorRanges(hsvRanges);
    std::vector<cv::Scalar> maxColorRanges = getMaxColorRanges(hsvRanges);
    std::vector<QString> minColorLabels = getMinColorLabels(hsvRanges);
    std::vector<QString> maxColorLabels = getMaxColorLabels(hsvRanges);


    std::vector<cv::Mat> masks(minColorRanges.size());
    std::vector<cv::Mat> colorMasks(minColorRanges.size());
    cv::RNG rng(12345);

    cv::Mat finalRes(img.rows, img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    /*cv::imshow("black", finalRes);
    cv::waitKey(0);
    cv::destroyAllWindows();*/

    for(unsigned int i = 0; i < minColorRanges.size(); i++) {
        //Create masks
        cv::inRange(imgSegmented, minColorRanges[i], maxColorRanges[i], masks[i]);

        cv::dilate(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));
        cv::dilate(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));

        cv::erode(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));
        cv::erode(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));

        //Create color masks (single color images)
        QString colorLabel = minColorLabels[i];
        cv::Scalar color = getColor(colorLabel);
        if(colorLabel.contains("Gray") || colorLabel.contains("White") || colorLabel.contains("Black")) continue;
        cv::Mat colorMask(img.rows, img.cols, CV_8UC3, color);
        cv::Mat colorMaskHSV;

        cv::cvtColor(colorMask, colorMaskHSV, cv::COLOR_BGR2HSV);

        colorMasks[i] = colorMaskHSV;

        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours( masks[i], contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE );

        unsigned long totalArea = 0;
        for ( int i=0; i < static_cast<int>(contours.size()); i++ )
            totalArea += cv::contourArea(contours[i]);

        for ( int i=0; i < static_cast<int>(contours.size()); i++ ) {
            float perc = cv::contourArea(contours[i]) * 100.0 / totalArea;
            // look for hierarchy[i][3]!=-1, ie hole boundaries
            if ( hierarchy[i][3] != -1 && perc >= 0.3 && perc < 3 ) {
                //if (perc >= 0.1 && perc < 0.7 ) {
                // random colour
                cv::Scalar colour(0, 0, 255);
                cv::drawContours( finalRes, contours, i, colour, cv::FILLED );
            }
        }
    }
    return finalRes;
}


cv::Mat ImageAnalyzer::colorSegmentation2D(cv::Mat img) {
    cv::Mat imgSegmented, _image;
    img.copyTo(_image);
    //cv::GaussianBlur(imgSegmented, imgSegmented, cv::Size(15, 15), 0);


    cv::bilateralFilter ( _image, imgSegmented, 9, 9, 7 );
    cv::bilateralFilter ( imgSegmented, _image, 9, 9, 7 );
    cv::bilateralFilter ( _image, imgSegmented,  9, 9, 7 );
    cv::bilateralFilter ( imgSegmented, _image, 9, 9, 7 );
    cv::bilateralFilter ( _image, imgSegmented,  9, 9, 7 );

    cv::cvtColor(img, imgSegmented, cv::COLOR_BGR2HSV);

    std::map<QString, cv::Scalar> hsvRanges = generateHsvRanges();

    std::vector<cv::Scalar> minColorRanges = getMinColorRanges(hsvRanges);
    std::vector<cv::Scalar> maxColorRanges = getMaxColorRanges(hsvRanges);
    std::vector<QString> minColorLabels = getMinColorLabels(hsvRanges);
    std::vector<QString> maxColorLabels = getMaxColorLabels(hsvRanges);


    std::vector<cv::Mat> masks(minColorRanges.size());
    std::vector<cv::Mat> colorMasks(minColorRanges.size());
    cv::RNG rng(12345);

    for(unsigned int i = 0; i < minColorRanges.size(); i++) {
        //Create masks
        cv::inRange(imgSegmented, minColorRanges[i], maxColorRanges[i], masks[i]);

        //cv::Mat element = cv::getStructuringElement(0, cv::Size(3, 3));
        //cv::Mat element2 = cv::getStructuringElement(0, cv::Size(15, 15));

        //cv::morphologyEx(masks[i], masks[i], 3, element);
        //cv::morphologyEx(masks[i], masks[i], 2, element);

        cv::dilate(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));
        cv::dilate(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));

        cv::erode(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));
        cv::erode(masks[i], masks[i], cv::Mat::ones(3, 3, CV_8UC1));

        //Create color masks (single color images)
        QString colorLabel = minColorLabels[i];
        cv::Scalar color = getColor(colorLabel);


        cv::Mat colorMask(img.rows, img.cols, CV_8UC3, color);
        cv::Mat colorMaskHSV;
        cv::cvtColor(colorMask, colorMaskHSV, cv::COLOR_BGR2HSV);
        colorMasks[i] = colorMaskHSV;

        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(masks[i], contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        int totalArea = 0;
        cv::Mat finalImage(masks[i].rows, masks[i].cols, CV_8UC3);
        for(int i = 0; i < static_cast<int>(contours.size()); i++) {
            totalArea += cv::contourArea(contours[i]);
        }
        this->colorFrequency.insert(colorLabel, totalArea);
    }

    cv::Mat hsvResFinal = cv::Mat::zeros(colorMasks[0].rows, colorMasks[0].cols, CV_8UC3);
    for(unsigned int i = 0; i < minColorRanges.size(); i++) {
        cv::Mat hsvRes, hsvTemp;
        cv::bitwise_and(colorMasks[i], colorMasks[i], hsvRes, masks[i]);
        //cv::bilateralFilter ( hsvTemp, hsvRes, 15, 80, 80 );
        cv::GaussianBlur(hsvRes, hsvRes, cv::Size(5, 5), 0);
        hsvResFinal += hsvRes;
    }

    cv::cvtColor(hsvResFinal, hsvResFinal, cv::COLOR_HSV2BGR);

    return hsvResFinal;
}

QMap<QString, int> ImageAnalyzer::getColorFrequency() {
    return colorFrequency;
}



void ImageAnalyzer::size(cv::Mat img) {
    QString size = "(" + QString::number(img.size[0]) + ", " + QString::number(img.size[1]) + ")";
    std::cout << size.toUtf8().data() << std::endl;
}


cv::Scalar ImageAnalyzer::getColor(QString colorLabel) {
    cv::Scalar color = cv::Scalar(0, 255, 0);
    if(colorLabel == "Red_min" || colorLabel == "Red_2_min") {
        color = cv::Scalar(0, 0, 255);
    } else if(colorLabel == "Orange_min") {
        color = cv::Scalar(0, 75, 150);
    }  else if(colorLabel == "Yellow_min") {
        color = cv::Scalar(0, 255, 255);
    } else if(colorLabel == "Green_min") {
        color = cv::Scalar(0, 255, 0);
    } else if(colorLabel == "Blue_min") {
        color = cv::Scalar(255, 0, 0);
    } else if(colorLabel == "Purple_min") {
        //color = cv::Scalar(211, 0, 148);
        color = cv::Scalar(60, 20, 220);
    } else if(colorLabel == "White_min") {
        color = cv::Scalar(255, 255, 255);
    } else if(colorLabel == "Black_min") {
        color = cv::Scalar(0, 0, 0);
    } else if(colorLabel == "Gray_min") {
        color = cv::Scalar(50, 50, 50);
    }

    return color;
}


std::map<QString, cv::Scalar> ImageAnalyzer::generateHsvRanges(){
    std::map<QString, cv::Scalar> hsvRanges;

    int minS = 0;
    int maxS = 255;
    int minV = 0;
    int maxV = 255;


    hsvRanges.insert(std::pair<QString, cv::Scalar>("White_min", cv::Scalar(0, 0, 150)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("White_max", cv::Scalar(180, 35, 255)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Black_min", cv::Scalar(0, 0, 0)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Black_max", cv::Scalar(180, 70, 30)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Gray_min", cv::Scalar(0, 0, 40)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Gray_max", cv::Scalar(180, 50, 240)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_min", cv::Scalar(1, 85, 115)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_max", cv::Scalar(19, 255, 255)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Orange_min", cv::Scalar(0, 36, 86)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Orange_max", cv::Scalar(84, 85, 255)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Yellow_min", cv::Scalar(20, 85, 220)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Yellow_max", cv::Scalar(31, 255, 255)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Green_min", cv::Scalar(31, 85, 0)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Green_max", cv::Scalar(84, 255, 255)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Blue_min", cv::Scalar(85, 35, 105)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Blue_max", cv::Scalar(142, 255, 255)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_2_min", cv::Scalar(142, 40, 40)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_2_max", cv::Scalar(180, 255, 255)));

    //hsvRanges.insert(std::pair<QString, cv::Scalar>("Purple_min", cv::Scalar(139, 35, 0)));
    //hsvRanges.insert(std::pair<QString, cv::Scalar>("Purple_max", cv::Scalar(149, 255, 255)));

    /*
    */

    /*
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_min", cv::Scalar(0, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_max", cv::Scalar(6, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_orange_min", cv::Scalar(7, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red_orange_max", cv::Scalar(11, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Orange_brown_min", cv::Scalar(12, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Orange_brown_max", cv::Scalar(20, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Orange_yellow_min", cv::Scalar(21, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Orange_yellow_max", cv::Scalar(25, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Yellow_min", cv::Scalar(26, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Yellow_max", cv::Scalar(32, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Yellow_green_min", cv::Scalar(33, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Yellow_green_max", cv::Scalar(40, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Green_min", cv::Scalar(41, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Green_max", cv::Scalar(67, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Green_cyan_min", cv::Scalar(68, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Green_cyan_max", cv::Scalar(85, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Cyan_min", cv::Scalar(86, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Cyan_max", cv::Scalar(100, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Cyan_blue_min", cv::Scalar(101, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Cyan_blue_max", cv::Scalar(110, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Blue_min", cv::Scalar(111, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Blue_max", cv::Scalar(121, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Blue_purple_min", cv::Scalar(122, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Blue_purple_max", cv::Scalar(140, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Purple_min", cv::Scalar(141, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Purple_max", cv::Scalar(160, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Purple_pink_min", cv::Scalar(161, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Purple_pink_max", cv::Scalar(165, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Pink_min", cv::Scalar(166, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Pink_max", cv::Scalar(173, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Pink_red_min", cv::Scalar(174, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Pink_red_max", cv::Scalar(177, maxSV, maxSV)));

    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red2_min", cv::Scalar(170, minSV, minSV)));
    hsvRanges.insert(std::pair<QString, cv::Scalar>("Red2_max", cv::Scalar(180, maxSV, maxSV)));
*/
    return hsvRanges;
}


int ImageAnalyzer::calculateMaxHistogramValue(cv::Mat img, bool display) {
    cv::Mat imgHsv;
    std::vector<cv::Mat> hsvPlanes;
    cv::cvtColor(img, imgHsv, cv::COLOR_BGR2HSV);
    cv::split(imgHsv, hsvPlanes);

    int histSize = 180;
    float range[] = { 1, 181 } ;

    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    cv::Mat h_hist;

    /// Compute the histograms:
    cv::calcHist( &hsvPlanes[0], 1, nullptr, cv::Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histogram
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( static_cast<double>( hist_w )/histSize );

    cv::Mat histImage( hist_h, hist_w, CV_8UC3,cv:: Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    cv::normalize(h_hist, h_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ ) {

        line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(h_hist.at<float>(i-1)) ) ,
              cv::Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i)) ),
              cv::Scalar( 255, 0, 0), 2, 8, 0  );
    }

    // Display
    if(display) {
        cv::namedWindow("calcHist Demo", cv::WINDOW_AUTOSIZE );
        cv::imshow("calcHist Demo", histImage );
        cv::waitKey(0);
    }
    std::vector<float> histValues;

    for(int i = 0; i < h_hist.rows; i++){
        histValues.push_back(h_hist.at<float>(i,0));
    }

    int maxElement = int(std::max_element(histValues.begin(),histValues.end()) - histValues.begin());
    return maxElement;
}


QMap<QString, int> ImageAnalyzer::calculateColorHistogram(QString fullpath) {
    cv::Mat img = cv::imread(fullpath.toUtf8().data());
    cv::Mat imgHsv;

    cv::cvtColor(img, imgHsv, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> hsvPlanes;
    cv::split(imgHsv, hsvPlanes);

    int histSize = 180;
    float range[] = { 1, 181 } ;

    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    cv::Mat h_hist;

    /// Compute the histograms:
    cv::calcHist( &hsvPlanes[0], 1, nullptr, cv::Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate );

    std::vector<int> histValues;
    QMap<QString, int> output;

    for(int i = 0; i < h_hist.rows; i++) {
        int value = static_cast<int>(h_hist.at<float>(i,0));

        if (value < 0) value = 0.0;

        if((i >= 0 && i <= 6) || (i > 177 && i <= 180)) {
            output.insert("Red", value);
        } else if (i > 6 && i <= 11 ) {
            output.insert("Red-orange", value);
        } else if (i > 11 && i <= 20 ) {
            output.insert("Orange-brown", value);
        } else if (i > 20 && i <= 25 ) {
            output.insert("Orange-yellow", value);
        } else if (i > 25 && i <= 32 ) {
            output.insert("Yellow", value);
        } else if (i > 32 && i <= 40 ) {
            output.insert("Yellow-green", value);
        } else if (i > 40 && i <= 67 ) {
            output.insert("Green", value);
        } else if (i > 67 && i <= 85 ) {
            output.insert("Green-cyan", value);
        } else if (i > 85 && i <= 100 ) {
            output.insert("Cyan", value);
        } else if (i > 100 && i <= 110 ) {
            output.insert("Cyan-blue", value);
        } else if (i > 110 && i <= 121 ) {
            output.insert("Blue", value);
        } else if (i > 121 && i <= 140 ) {
            output.insert("Blue-purple", value);
        } else if (i > 140 && i <= 160 ) {
            output.insert("Purple", value);
        } else if (i > 160 && i <= 165 ) {
            output.insert("Purple-pink", value);
        } else if (i > 165 && i <= 173 ) {
            output.insert("Pink", value);
        } else if (i > 173 && i <= 177 ) {
            output.insert("Pink-red", value);
        } else {
            qDebug() << "Other:" << value;
            output.insert("Unknown color", value);
        }
    }

    return output;

}


std::vector<cv::Scalar> ImageAnalyzer::getMinColorRanges(std::map<QString, cv::Scalar> hsvRanges) {
    std::map<QString, cv::Scalar>::iterator itr;
    std::vector<cv::Scalar> minColorRanges;
    int i = 0;

    for(itr = hsvRanges.begin(); itr != hsvRanges.end(); ++itr) {
        if(i % 2 != 0) minColorRanges.push_back(itr->second);
        i++;
    }

    return minColorRanges;
}


std::vector<cv::Scalar> ImageAnalyzer::getMaxColorRanges(std::map<QString, cv::Scalar> hsvRanges) {
    std::map<QString, cv::Scalar>::iterator itr;
    std::vector<cv::Scalar> maxColorRanges;

    int i = 0;

    for(itr = hsvRanges.begin(); itr != hsvRanges.end(); ++itr) {
        if(i % 2 == 0) maxColorRanges.push_back(itr->second);
        i++;
    }

    return maxColorRanges;
}


std::vector<QString> ImageAnalyzer::getMinColorLabels(std::map<QString, cv::Scalar> hsvRanges) {
    std::map<QString, cv::Scalar>::iterator itr;
    std::vector<QString> minColorLabels;
    int i = 0;

    for(itr = hsvRanges.begin(); itr != hsvRanges.end(); ++itr) {
        if(i % 2 != 0) minColorLabels.push_back(itr->first);
        i++;
    }

    return minColorLabels;
}


std::vector<QString> ImageAnalyzer::getMaxColorLabels(std::map<QString, cv::Scalar> hsvRanges) {
    std::map<QString, cv::Scalar>::iterator itr;
    std::vector<QString> maxColorLabels;

    int i = 0;

    for(itr = hsvRanges.begin(); itr != hsvRanges.end(); ++itr) {
        if(i % 2 == 0) {
            maxColorLabels.push_back(itr->first);
        }
        i++;
    }

    return maxColorLabels;
}
