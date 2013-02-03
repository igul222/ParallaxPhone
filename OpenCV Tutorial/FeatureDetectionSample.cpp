//
//  FeatureDetection.cpp
//  OpenCV Tutorial
//
//  Created by Anton Belodedenko on 25/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "FeatureDetectionSample.h"

#define kDetectorORB  "ORB"
#define kDetectorSURF "SURF"
#define kDetectorFAST "FAST"

FeatureDetectionSample::FeatureDetectionSample()
    : m_maxFeatures(100)
    , m_fastThreshold(10)
{
    // feature extraction options
    m_alorithms.push_back( kDetectorORB );
    m_alorithms.push_back( kDetectorFAST );
    m_alorithms.push_back( kDetectorSURF );
    
    registerOption("Detection algorithm", "", &m_detectorName, m_alorithms);
    registerOption("Max features",        "", &m_maxFeatures, 1, 100);
    
    
    registerOption("Threshold", "FAST", &m_fastThreshold, 1, 100);
}

//! Gets a sample name
std::string FeatureDetectionSample::getName() const
{
    return "Feature detection";
}

std::string FeatureDetectionSample::getSampleIcon() const
{
    return "FeatureDetectionSampleIcon.png";
}

//! Returns a detailed sample description
std::string FeatureDetectionSample::getDescription() const
{
    return "Feature detection sample.";
}

static bool keypoint_score_greater(const cv::KeyPoint& kp1, const cv::KeyPoint& kp2)
{
    return kp1.response > kp2.response;
}

//! Processes a frame and returns output image
bool FeatureDetectionSample::processFrame(const cv::Mat& inputFrame, cv::Mat& outputFrame)
{
    cv::Mat hsv;
    cv::cvtColor(inputFrame, hsv, CV_BGR2HSV);
    
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    
    cv::Mat computation = channels[0].mul(channels[1]/255.0).mul(channels[2]/255.0);
    
    int brightest_x = 0;
    int brightest_y = 0;
    int brightest_val = 0;
    for(int x = 0; x < computation.cols; x++) {
        for(int y = 0; y < computation.rows; y++) {
            if(computation.at<uchar>(y,x) > brightest_val) {
                brightest_val = computation.at<uchar>(y,x);
                brightest_x = x;
                brightest_y = y;
            }
        }
    }
    
    objectKeypoints.clear();
    objectKeypoints.push_back(cv::KeyPoint(brightest_x, brightest_y, 50));
    
    cv::Mat blur;
    cv::blur(computation, blur, cv::Size(5,5));
    blur.copyTo(computation);
    
    cv::Mat t;
    cv::cvtColor(computation, t, CV_GRAY2BGR);
    cv::drawKeypoints(t, objectKeypoints, t, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    
    cv::cvtColor(t, outputFrame, CV_BGR2BGRA);
    
    topX = brightest_x;
    topY = brightest_y;
    
    return true;
}