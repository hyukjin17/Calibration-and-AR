/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Using the 3D points defined by the TargetDetector, calibrates the camera
 */

#include <iostream>
#include <filesystem>
#include "Calibrator.hpp"
#include "TargetDetector.hpp"

// Constructor
Calibrator::Calibrator()
{
    distCoef = cv::Mat::zeros(8, 1, CV_64FC1);
}

// Sets the image size using the given dimensions
void Calibrator::setImgSize(int cols, int rows)
{
    imgSize = cv::Size(cols, rows);
}

// Clear variables
void Calibrator::clear()
{
    pointList.clear();
    cornerList.clear();
    numCalFrames = 0;
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

// Calibrate the camera matrix and distortion coefficients
// Print out the initial values and after calibration values
// Also prints out the reprojection error
void Calibrator::calibrate(int cols, int rows)
{
    if (cornerList.size() < 5)
    {
        std::cout << "Please take at least 5 calibration images" << std::endl;
        return;
    }
    
    setImgSize(cols, rows);
    setCameraMatrix(cols, rows);

    // Initial calibration variables
    std::cout << "Camera Matrix: " << std::endl << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << std::endl << distCoef << std::endl;

    reprojError = cv::calibrateCamera(pointList, cornerList, imgSize, cameraMatrix, distCoef, rvecs, tvecs, flags);
    // Calibration variables after calibration
    std::cout << "Camera Matrix: " << std::endl << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << std::endl << distCoef << std::endl;
    std::cout << "Reprojection Error: " << reprojError << std::endl;

}

// Load all calibration points from calibration images in the output folder into memory
void Calibrator::loadExistingCalImages(std::string calImgFolder, int &savedCount)
{
    TargetDetector td;

    if (std::filesystem::exists(calImgFolder))
    {
        std::cout << "Scanning for existing calibration images..." << std::endl;
        for (const auto& entry : std::filesystem::directory_iterator(calImgFolder))
        {
            // Only process actual files (skip sub-folders)
            if (entry.is_regular_file())
            {
                cv::Mat img = cv::imread(entry.path().string());
                if (!img.empty())
                {
                    // If markers are found, add them to the calibrator
                    if (td.processFrame(img))
                    {
                        addCornersPoints(td.getFlattenedCorners(), td.computePointSet());
                        savedCount++; 
                    }
                }
            }
        }
        std::cout << "Loaded " << getNumCalFrames() << " previously saved frames." << std::endl;
    }
}

// Save the calibration parameters and camera position estimation to .yml files
void Calibrator::saveCalibration(const std::string &filename1, const std::string& filename2)
{
    // Open a file in WRITE mode for calibration parameters
    cv::FileStorage fs(filename1, cv::FileStorage::WRITE);
    
    if (!fs.isOpened()) {
        std::cerr << "Failed to open file: " << filename1 << std::endl;
        return;
    }

    // Write the variables into the file
    fs << "cameraMatrix" << cameraMatrix;
    fs << "distCoeffs" << distCoef;
    fs << "reprojectionError" << reprojError;
    fs.release();

    // Open another file in WRITE mode for camera parameters
    cv::FileStorage fs2(filename2, cv::FileStorage::WRITE);
    
    if (!fs2.isOpened()) {
        std::cerr << "Failed to open file: " << filename2 << std::endl;
        return;
    }

    fs2 << "rvecs" << rvecs;
    fs2 << "tvecs" << tvecs;
    fs2.release();

    std::cout << "Calibration saved to: " << filename1 << std::endl;
    std::cout << "Camera position and rotation saved to: " << filename2 << std::endl;
}

// Load the calibration information from a .yml file
bool Calibrator::loadCalibration(const std::string &filename)
{
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    fs["cameraMatrix"] >> cameraMatrix;
    fs["distCoeffs"] >> distCoef;
    fs.release();
    
    std::cout << "Successfully loaded intrinsic parameters from " << filename << std::endl;
    return true;
}