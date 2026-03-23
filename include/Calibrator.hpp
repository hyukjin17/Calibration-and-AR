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
    int getNumCalFrames() const;

    // Populates the list with all points identified in calibration images
    void addCornersPoints(const std::vector<cv::Point2f> &cornerSet, const std::vector<cv::Vec3f> &pointSet);
    // Calibrate the camera matrix
    void calibrate(int cols, int rows);

private:
	std::vector<std::vector<cv::Vec3f>> pointList;
	std::vector<std::vector<cv::Point2f>> cornerList;
    int numCalFrames = 0;

    // Calibration parameters
    cv::Size imgSize;
    cv::Mat distCoef;
    cv::Mat cameraMatrix;
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;
    // flags assume square pixels, use the initial camera matrix, and keep all 8 values in the distortion coefficient vector
    int flags = cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_USE_INTRINSIC_GUESS | cv::CALIB_RATIONAL_MODEL;

    // Setter functions
    void setImgSize(int cols, int rows);
    void setCameraMatrix(int cols, int rows);
};