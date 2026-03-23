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
#include "TargetDetector.hpp"
#include "Calibrator.hpp"

int main(int argc, char *argv[])
{
    cv::Mat src, dst;
    TargetDetector td;
    Calibrator cal;
    int savedCount = 1;

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
        cv::imshow("ArUco Detection Test", dst);
        // std::cout << "Number of markers: " << td.getCornerSet().size() << std::endl;

        char key = cv::waitKey(1);
        if (key == 'q')
            break;
        if (key == 's')
        {
            if (targetDetected)
            {
                // convert the corners into the 
                cal.addCornersPoints(td.getFlattenedCorners(), td.computePointSet());
                std::filesystem::create_directories("data/calibration_frames");
                std::string filename = "data/calibration_frames/cal_" + std::to_string(savedCount++) + ".jpg";
                cv::imwrite(filename, dst);
                std::cout << "Calibration image saved!" << std::endl;
            }
            else
            {
                std::cout << "No ArUco markers detected!" << std::endl;
            }
        }
    }

    return (0);
}