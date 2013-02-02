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
static cv::Mat prevImage;
static int runs = 0;
bool FeatureDetectionSample::processFrame(const cv::Mat& inputFrame, cv::Mat& outputFrame)
{
    // convert input frame to gray scale
    cv::Mat currentGrayImage;
    getGray(inputFrame, currentGrayImage);
    
    if(runs == 0 || runs == 10) {
        getGray(inputFrame, prevImage);
        printf("setting prevImage\n");
        
    }
    runs += 1;
    
    cv::Mat grayImage1 = prevImage - currentGrayImage;
    cv::Mat grayImage2 = currentGrayImage - prevImage;
    grayImage = grayImage1 + grayImage2;
    
    cv::Mat blurGrayImage;
    cv::boxFilter(grayImage, blurGrayImage, 0, cv::Size (10,10));
    blurGrayImage.copyTo(grayImage);
    
    cv::Mat thresholdGrayImage;
    cv::threshold(grayImage, thresholdGrayImage, 20, 255, 0);
//    thresholdGrayImage.copyTo(grayImage);
    
    cv::Mat edges;
    cv::Canny(thresholdGrayImage, edges, 50, 150);
    
    std::vector< std::vector<cv::Point> > contours;
    
    cv::findContours(edges, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    printf("CONTOURS SIZE IS %lu\n",contours.size());
    
    objectKeypoints.clear();
    
    for(int i=0; i<contours.size(); i++) {
        for(int j=0; j<contours[i].size(); j++) {
            cv::KeyPoint pt (contours[i][j].x,contours[i][j].y,5);
            objectKeypoints.push_back(pt);
        }
    }

    cv::Mat t;
    cv::cvtColor(grayImage, t, CV_GRAY2BGR);
    cv::drawKeypoints(t, objectKeypoints, t, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    
    cv::cvtColor(t, outputFrame, CV_BGR2BGRA);
    
//    currentGrayImage.copyTo(prevImage);
    return true;
}