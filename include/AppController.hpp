/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Header file for the Controller (used for detecting user key inputs)
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <filesystem>
#include "config.hpp"
#include "Calibrator.hpp"
#include "TargetDetector.hpp"

class AppController
{
public:
    // Constructor
    AppController();

    int getSavedCount() { return savedCount; }
    void setSavedCount(int num) { savedCount = num; }

    // Handles user input (saving calibration images, calibrating camera, etc.)
    // Returns true if the user presses 'q' to quit
    bool handleInput(char key, cv::Mat &src, cv::Mat &dst, Calibrator &cal, TargetDetector &td, bool targetDetected);

private:
    int savedCount = 1;
};