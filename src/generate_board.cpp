/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Generates a grid of ArUco markers to be used for camera calibration
 * There are no available boards online with defined distances, so custom board was needed
 */

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <iostream>
#include "TargetDetector.hpp"

int main()
{
    TargetDetector td; // used to extract grid size and marker size information

    // use the same dictionary as the detector
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    // generate the ArUco board and save it as a png
    cv::aruco::GridBoard board(td.getGridSize(), td.getMarkerLength(), td.getMarkerSep(), dictionary);
    cv::Mat boardImage;
    board.generateImage(cv::Size(2800, 2000), boardImage, 50, 1);
    cv::imwrite("ArUco_Board.png", boardImage);

    std::cout << "Successfully generated and saved ArUco_Board.png!" << std::endl;
    return 0;
}