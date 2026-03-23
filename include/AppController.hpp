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
#include "Renderer.hpp"

class AppController
{
public:
    // Constructor
    AppController();

    // Getters
    int getSavedCount() const { return savedCount; }
    bool getShowCrystal() const { return showCrystal; }
    bool getShowAxes() const { return showAxes; }
    bool getShowObject() const { return showObject; }

    // Setters
    void setSavedCount(int num) { savedCount = num; }

    // Handles user input (saving calibration images, calibrating camera, etc.)
    // Returns true if the user presses 'q' to quit
    bool handleInput(char key, cv::Mat &src, cv::Mat &dst, Calibrator &cal,
                     TargetDetector &td, bool targetDetected);

private:
    int savedCount = 1;
    bool showCrystal = false; // display crystal?
    bool showAxes = true;     // display axes?
    bool showObject = false;  // display virtual object?
};