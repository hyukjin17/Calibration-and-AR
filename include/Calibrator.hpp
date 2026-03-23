/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Header file for the Calibrator (used for camera calibration)
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>

class Calibrator {
public:
    // Constructor
    Calibrator();

    // Getter functions
    std::vector<std::vector<cv::Vec3f>> getPointList() const;
    std::vector<std::vector<cv::Point2f>> getCornerList() const;
    void addCornersPoints(const std::vector<cv::Point2f>& cornerSet, const std::vector<cv::Vec3f>& pointSet);

private:
	std::vector<std::vector<cv::Vec3f>> pointList;
	std::vector<std::vector<cv::Point2f>> cornerList;
};