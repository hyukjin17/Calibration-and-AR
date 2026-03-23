/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Header file for the Calibrator (used for camera calibration)
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <iostream>
#include <filesystem>
#include "TargetDetector.hpp"

class Calibrator
{
public:
    // Constructor
    Calibrator();

    // Getter functions
    std::vector<std::vector<cv::Vec3f>> getPointList() const { return pointList; }
    std::vector<std::vector<cv::Point2f>> getCornerList() const { return cornerList; }
    int getNumCalFrames() const { return numCalFrames; }
    cv::Mat getCameraMatrix() const { return cameraMatrix; }
    cv::Mat getDistCoef() const { return distCoef; }

    // Populates the list with all points identified in calibration images
    void addCornersPoints(const std::vector<cv::Point2f> &cornerSet, const std::vector<cv::Vec3f> &pointSet);
    // Calibrate the camera matrix
    void calibrate(int cols, int rows);
    // Clear variables
    void clear();
    // Load all points from calibration images in the output folder into memory
    // Return number of images loaded
    int loadExistingCalImages(std::string calImgFolder);
    // Save the calibration parameters and camera position estimation to .yml files
    void saveCalibration(const std::string &filename1, const std::string &filename2);
    // Load calibration parameters from a file
    bool loadCalibration(const std::string &filename);

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
    double reprojError;
    // flags assume square pixels
    int flags = cv::CALIB_FIX_ASPECT_RATIO;

    // Setter functions
    void setImgSize(int cols, int rows);
    void setCameraMatrix(int cols, int rows);
};