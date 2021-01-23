#include "changedetector.h"

ChangeDetector::ChangeDetector()
{

}

cv::Mat ChangeDetector::findDifferences(cv::Mat img1, cv::Mat img2) {
    ImageAnalyzer imageAnalyzer;

    cv::Mat im1 = img1.clone();
    cv::Mat im2 = img2.clone();

    im1 = imageAnalyzer.colorSegmentation2D(im1);
    im2 = imageAnalyzer.colorSegmentation2D(im2);

    cv::Mat diff, diffGray, finalMask;
    cv::absdiff(im1, im2, diff);
    cv::cvtColor(diff, diffGray, cv::COLOR_BGR2GRAY);

    cv::threshold(diffGray, finalMask, 10, 255, 0);

    //cv::dilate(finalMask, finalMask, cv::Mat::ones(3, 3, CV_8UC1));

    cv::erode(finalMask, finalMask, cv::Mat::ones(3, 3, CV_8UC1));

    cv::dilate(finalMask, finalMask, cv::Mat::ones(3, 3, CV_8UC1));
    cv::dilate(finalMask, finalMask, cv::Mat::ones(3, 3, CV_8UC1));

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours( finalMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    std::vector<cv::Rect> boundRect( contours.size() );
    std::vector<cv::Point2f>center( contours.size() );
    std::vector<float>radius( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ ) {
        cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( cv::Mat(contours_poly[i]) );
        //std::cout<<"Countour area: " << boundRect[i].height * boundRect[i].width << std::endl;
        //std::cout<<"Countour area: " << cv::contourArea(boundRect[i]) << std::endl;
    }

    cv::Mat drawing = cv::Mat::zeros( finalMask.size(), CV_8UC3 );

    for( size_t i = 0; i< contours.size(); i++ ) {
        int bbArea = boundRect[i].height * boundRect[i].width;
        double bbAreaPercentage = bbArea * 100.0 / (finalMask.rows * finalMask.cols);
        if(bbAreaPercentage > 0.1) {
            //drawContours( drawing, contours_po§ly, (int)i, color, 0, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
            cv::rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 0, 255), 5, 8, 0 );
            //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
        }
    }

    cv::Mat final = drawing + img2;
    return drawing;

    //cv::Mat im1 = imgs[0];
    //cv::Mat im2 = imgs[1];

    int minHessian = 400;
    //create SURF object

    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);

    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    detector->detectAndCompute( im1, cv::noArray(), keypoints1, descriptors1 );
    detector->detectAndCompute( im2, cv::noArray(), keypoints2, descriptors2 );

    //SURF
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);

    std::vector< std::vector<cv::DMatch> > knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

    //Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.75f;
    std::vector<cv::DMatch> goodMatches;

    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            goodMatches.push_back(knn_matches[i][0]);
        }
    }

    //Draw matches
    cv::Mat imgMatches;
    cv::drawMatches( im1, keypoints1, im2, keypoints2, goodMatches, imgMatches, cv::Scalar::all(-1),
                     cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //Localize the object
    std::vector<cv::Point2f> kp1;
    std::vector<cv::Point2f> kp2;
    for( size_t i = 0; i < goodMatches.size(); i++ ){
        //Get the keypoints from the good matches
        kp1.push_back( keypoints1[unsigned(goodMatches[i].queryIdx) ].pt );
        kp2.push_back( keypoints2[unsigned(goodMatches[i].trainIdx) ].pt );
    }

    //Finding Homography
    cv::Mat H = findHomography(kp1, kp2, cv::RANSAC);

    //Show detected matches
    //cv::imshow("Good Matches & Object detection", imgMatches );

    cv::Mat img2Transformed, imOut2;
    //Warp source image to destination based on homography
    cv::warpPerspective(im2, img2Transformed, H, im1.size());

    cv::absdiff(im1, img2Transformed, imOut2);
    cv::Mat finalIm2 = cv::Mat::zeros(im2.rows, im2.cols, CV_8UC3);

    //cv::imshow("Result (pre)", imgOut2);
    cv::threshold(imOut2, imOut2, 35, 255, cv::THRESH_BINARY);
    cv::erode(imOut2, imOut2, cv::Mat::ones(3, 3, CV_8UC1));

    cv::bitwise_and(im2, im2, finalIm2, imOut2);

    return finalIm2;

}
