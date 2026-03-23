/**
 * Hyuk Jin Chung
 * 3/21/2026
 * 
 * Header file for the TargetDetector
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include "config.hpp"

class TargetDetector {
public:
    // Constructor
    TargetDetector();

    // Returns true if at least one marker was found
    bool processFrame(cv::Mat& frame);

    // Getters for the data found in the most recent frame
    std::vector<std::vector<cv::Point2f>> getCornerSet() const { return cornerSet; }
    std::vector<int> getMarkerIDs() const { return markerIDs; }

    // Converts the corners into a vector of 3D world point
    std::vector<cv::Point2f> getFlattenedCorners() const;
    // Converts the 2D image coordinates to the 3D world coordinates for the detected corners
    std::vector<cv::Vec3f> computePointSet() const;

private:
    cv::aruco::Dictionary dictionary;
    cv::aruco::DetectorParameters detectorParams;
    cv::aruco::ArucoDetector detector;

    // Features for the current frame
    std::vector<int> markerIDs;
    std::vector<std::vector<cv::Point2f>> cornerSet;
    std::vector<std::vector<cv::Point2f>> rejectedCandidates;
};