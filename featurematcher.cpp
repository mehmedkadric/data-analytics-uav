#include "featurematcher.h"

#define MIN_MATCH_COUNT = 10

FeatureMatcher::FeatureMatcher()
{

}


cv::Mat matchFeatures(cv::Mat im1, cv::Mat im2) {
    cv::Mat img_mat;


    return im1 + im2;
}


cv::Mat FeatureMatcher::findFeatures(cv::Mat im1, cv::Mat im2) {

    cv::Mat img1, img2;

    cv::cvtColor(im1, img1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(im2, img2, cv::COLOR_BGR2GRAY);

    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    int minHessian = 400;
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian );
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    detector->detectAndCompute( img1, cv::noArray(), keypoints1, descriptors1 );
    detector->detectAndCompute( img2, cv::noArray(), keypoints2, descriptors2 );

    //-- Step 2: Matching descriptor vectors with a FLANN based matcher
    // Since SURF is a floating-point descriptor NORM_L2 is used
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<cv::DMatch> > knn_matches;
    matcher->knnMatch( descriptors1, descriptors2, knn_matches, 2 );

    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    std::vector<cv::DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++){
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    //-- Draw matches
    cv::Mat img_matches;
    cv::drawMatches( img1, keypoints1, img2, keypoints2, good_matches, img_matches, cv::Scalar::all(-1),
                     cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //Localize the object
    std::vector<cv::Point2f> kp1;
    std::vector<cv::Point2f> kp2;
    for( int i = 0; i < good_matches.size(); i++ ){
        //Get the keypoints from the good matches
        kp1.push_back( keypoints1[unsigned(good_matches[i].queryIdx) ].pt );
        kp2.push_back( keypoints2[unsigned(good_matches[i].trainIdx) ].pt );
    }
    //new camera_matrix = cv::getOptimalNewCameraMatrix();
    //Finding Homography
    cv::Mat H = findHomography(kp1, kp2, cv::RANSAC);

    //Show detected matches
    //cv::imshow("Good Matches & Object detection", img_matches );
    //cv::waitKey(0);

    cv::Mat img2Transformed;
    //Warp source image to destination based on homography
    cv::warpPerspective(im2, img2Transformed, H, im1.size());
    return img2Transformed;
}
