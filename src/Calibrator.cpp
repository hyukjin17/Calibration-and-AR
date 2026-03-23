/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Using the 3D points defined by the TargetDetector, calibrates the camera
 */

#include <iostream>
#include "Calibrator.hpp"

// Constructor
Calibrator::Calibrator()
{
    distCoef = cv::Mat::zeros(8, 1, CV_64FC1);
}

// Getter functions
std::vector<std::vector<cv::Vec3f>> Calibrator::getPointList() const
{
    return pointList;
}

std::vector<std::vector<cv::Point2f>> Calibrator::getCornerList() const
{
    return cornerList;
}

int Calibrator::getNumCalFrames() const
{
    return numCalFrames;
}

// Sets the image size using the given dimensions
void Calibrator::setImgSize(int cols, int rows)
{
    imgSize = cv::Size(cols, rows);
}

/**
 * Sets the initial camera matrix as shown below
 * 
 * [1, 0, frame.cols/2]
 * [0, 1, frame.rows/2]
 * [0, 0, 1           ]
 */
void Calibrator::setCameraMatrix(int cols, int rows)
{
    cameraMatrix = cv::Mat::eye(3, 3, CV_64FC1);
    cameraMatrix.at<double>(0, 2) = cols / 2.0;
    cameraMatrix.at<double>(1, 2) = rows / 2.0;
}

// Adds extracted corner points from the calibration image into the combined list
void Calibrator::addCornersPoints(const std::vector<cv::Point2f> &cornerSet, const std::vector<cv::Vec3f> &pointSet)
{
    cornerList.push_back(cornerSet);
    pointList.push_back(pointSet);
    numCalFrames++;
}

void Calibrator::calibrate(int cols, int rows)
{
    if (cornerList.size() < 5)
    {
        std::cout << "Please take at least 5 calibration images" << std::endl;
        return;
    }
    
    setImgSize(cols, rows);
    setCameraMatrix(cols, rows);

    std::cout << "Camera Matrix: " << std::endl << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << std::endl << distCoef << std::endl;
    double reprojError = cv::calibrateCamera(pointList, cornerList, imgSize, cameraMatrix, distCoef, rvecs, tvecs, flags);
    std::cout << "Camera Matrix: " << std::endl << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << std::endl << distCoef << std::endl;
    std::cout << "Reprojection Error: " << reprojError << std::endl;

}