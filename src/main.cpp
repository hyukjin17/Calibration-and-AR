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
#include "config.hpp"

int main(int argc, char *argv[])
{
    cv::Mat src, dst;
    TargetDetector td;
    Calibrator cal;
    int savedCount = 1;

    cal.loadExistingCalImages(Config::CAL_IMG_FOLDER, savedCount);

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
        else if (key == 's')
        {
            if (targetDetected)
            {
                // convert the corners into the 
                cal.addCornersPoints(td.getFlattenedCorners(), td.computePointSet());
                std::filesystem::create_directories(Config::CAL_IMG_FOLDER);
                std::filesystem::create_directories(Config::VIS_IMG_FOLDER);
                std::string filename = Config::CAL_IMG_FOLDER + "/cal_" + std::to_string(savedCount) + ".jpg";
                cv::imwrite(filename, src);
                std::string filename_1 = Config::VIS_IMG_FOLDER + "/vis_" + std::to_string(savedCount++) + ".jpg";
                cv::imwrite(filename_1, dst);
                std::cout << "Calibration image saved!" << std::endl;
            }
            else
            {
                std::cout << "No ArUco markers detected!" << std::endl;
            }
        }
        else if (key == 'c')
        {
            cal.calibrate(dst.cols, dst.rows);
            cal.saveCalibration(Config::CALIBRATION_FILE);
        }
        else if (key == 'd') 
        {
            if (std::filesystem::exists(Config::CAL_IMG_FOLDER))
            {
                // delete all image files in the folder
                for (const auto& entry : std::filesystem::directory_iterator(Config::CAL_IMG_FOLDER))
                {
                    std::filesystem::remove(entry.path());
                }
            }
            
            cal.clear(); 
            savedCount = 1; // Reset the naming counter
            
            std::cout << "All calibration images deleted" << std::endl;
        }
    }

    return (0);
}