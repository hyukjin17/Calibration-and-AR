/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Main entry point for the program
 * Runs the video loop and creates the AR environment from calibration to virtual object placement
 */

#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "TargetDetector.hpp"
#include "Calibrator.hpp"
#include "Renderer.hpp"
#include "AppController.hpp"
#include "config.hpp"

int main(int argc, char *argv[])
{
    cv::Mat src, dst;
    TargetDetector td;
    Calibrator cal;
    Renderer r;
    AppController controller;

    // Mute all OpenCV Info and Warning messages
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    int numImages = cal.loadExistingCalImages(Config::CAL_IMG_FOLDER);
    controller.setSavedCount(numImages);
    bool isCalibrated = cal.loadCalibration(Config::CALIBRATION_FILE);

    cv::VideoCapture capdev(0);
    if (!capdev.isOpened())
    {
        printf("Unable to open camera device\n");
        return (-1);
    }

    for (;;)
    {
        capdev >> src; // get a new frame from the camera, treat as a stream
        if (src.empty())
        {
            printf("Frame is empty\n");
            break;
        }

        src.copyTo(dst);
        bool targetDetected = td.processFrame(dst);

        if (targetDetected && isCalibrated)
        {
            std::vector<cv::Vec3f> pointSet = td.computePointSet();
            std::vector<cv::Point2f> cornerSet = td.getFlattenedCorners();

            // Solve for the current camera position
            cv::Mat rvec, tvec;
            bool success = cv::solvePnP(pointSet, cornerSet, cal.getCameraMatrix(), cal.getDistCoef(), rvec, tvec);

            if (success)
            {
                // draw a wireframe crystal near the origin (as a floating virtual object)
                r.drawCrystal(dst, rvec, tvec, cal.getCameraMatrix(), cal.getDistCoef());
                // draw 3D axes at the origin
                r.drawAxes(dst, rvec, tvec, cal.getCameraMatrix(), cal.getDistCoef(), Config::AXIS_LENGTH);
            }

            // Print the real-time movement data
            // std::cout << "Translation (tvec): [" << tvec.at<double>(0) << ", "
            //           << tvec.at<double>(1) << ", " << tvec.at<double>(2) << "]" << std::endl;
            // std::cout << "Rotation (rvec): [" << rvec.at<double>(0) << ", "
            //           << rvec.at<double>(1) << ", " << rvec.at<double>(2) << "]" << std::endl;
        }

        cv::imshow("ArUco Detection Test", dst);
        char key = cv::waitKey(1);
        bool quit = controller.handleInput(key, src, dst, cal, td, targetDetected);
        if (quit)
            break;
    }

    return (0);
}