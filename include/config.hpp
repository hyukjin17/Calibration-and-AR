/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Config variables
 * Main access point for global variables
 */

#pragma once

namespace Config {
    // Physical ArUco Board Settings
    inline const float MARKER_LENGTH = 1.0f;
    inline const float MARKER_SEPARATION = 0.2f; 
    inline const int COLS = 9;
    inline const int ROWS = 6;

    // File paths
    inline const std::string CAL_IMG_FOLDER = "data/calibration_frames";
    inline const std::string VIS_IMG_FOLDER = "data/bounding_box_vis";
    inline const std::string CALIBRATION_FILE = "data/calibration_params.yml";
    inline const std::string CAMERA_POSITION_FILE = "data/camera_position_params.yml";
}