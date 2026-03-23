/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Handles all user key inputs
 */

#include "AppController.hpp"

// Constructor
AppController::AppController() {}

// Handles user input (saving calibration images, calibrating camera, etc.)
// Returns true if the user presses 'q' to quit
bool AppController::handleInput(char key, cv::Mat &src, cv::Mat &dst, Calibrator &cal, TargetDetector &td, bool targetDetected)
{
    if (key == 'q')
        return true;
    else if (key == 's')
    {
        if (targetDetected)
        {
            // convert the detected corners into the list of feature points
            cal.addCornersPoints(td.getFlattenedCorners(), td.computePointSet());
            std::filesystem::create_directories(Config::CAL_IMG_FOLDER);
            std::filesystem::create_directories(Config::VIS_IMG_FOLDER);
            // save calibration images to the data folder
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
        // calibrate the camera using calibration data collected from images
        cal.calibrate(dst.cols, dst.rows);
        cal.saveCalibration(Config::CALIBRATION_FILE, Config::CAMERA_POSITION_FILE);
    }
    else if (key == 'd')
    {
        if (std::filesystem::exists(Config::CAL_IMG_FOLDER))
        {
            // delete all image files in the folder
            for (const auto &entry : std::filesystem::directory_iterator(Config::CAL_IMG_FOLDER))
            {
                std::filesystem::remove(entry.path());
            }
        }

        cal.clear();    // clear calibrator memory
        savedCount = 1; // Reset the naming counter

        std::cout << "All calibration images deleted" << std::endl;
    }

    return false;
}