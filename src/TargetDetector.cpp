/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Target detector is the eye of the system
 * Detects ArUco markers in the image and converts them into 3D points in the world
 */

#include "TargetDetector.hpp"

// Constructor
TargetDetector::TargetDetector()
{
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    detectorParams = cv::aruco::DetectorParameters();
    detectorParams.cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX; // detect corners with subpixel accuracy
    detector = cv::aruco::ArucoDetector(dictionary, detectorParams);
}

// Process each frame to find the ArUco markers
// Returns true if at least one marker was found
bool TargetDetector::processFrame(cv::Mat &frame)
{
    // clear the vectors from the previous frame
    cornerSet.clear();
    markerIDs.clear();
    rejectedCandidates.clear();

    // finds the markers in the current image
    detector.detectMarkers(frame, cornerSet, markerIDs, rejectedCandidates);

    // draws the green outlines and ID numbers onto the output image
    if (!markerIDs.empty())
    {
        cv::aruco::drawDetectedMarkers(frame, cornerSet, markerIDs);
        return true;
    }

    return false;
}

// Getter for the current ArUco marker corner set
std::vector<std::vector<cv::Point2f>> TargetDetector::getCornerSet() const
{
    return cornerSet;
}

// Getter for the current ArUco marker IDs
std::vector<int> TargetDetector::getMarkerIDs() const
{
    return markerIDs;
}

// Flattens the 2D corner set into a vector
std::vector<cv::Point2f> TargetDetector::getFlattenedCorners() const
{
    std::vector<cv::Point2f> flattenedCorners;
    for (const auto &marker : cornerSet)
    {
        for (const auto &corner : marker)
        {
            flattenedCorners.push_back(corner);
        }
    }
    return flattenedCorners;
}

// Generates a flattened vector of 3D points from the corner set
// Uses the custon ArUco grid made using generate_board.cpp
// Using the known distances between markers, computes 3D locations of markers in world coordinates (in units of marker size)
std::vector<cv::Vec3f> TargetDetector::computePointSet() const
{
    std::vector<cv::Vec3f> pointSet;

    for (int id : markerIDs)
    {
        // ignore any markers outside of the created ArUco grid
        if (id >= (cols * rows) || id < 0)
        {
            continue;
        }

        // find the marker position on the grid using the ID
        int col = id % cols;
        int row = id / cols;

        // find the offset from the origin (top left point of the first marker)
        float startX = col * (markerLength + markerSeparation);
        float startY = row * (markerLength + markerSeparation);

        // define 4 corners based on the offset (in the right order)
        pointSet.push_back(cv::Vec3f(startX, startY, 0.0f));                               // Top left
        pointSet.push_back(cv::Vec3f(startX + markerLength, startY, 0.0f));                // Top right
        pointSet.push_back(cv::Vec3f(startX + markerLength, startY + markerLength, 0.0f)); // Bottom right
        pointSet.push_back(cv::Vec3f(startX, startY + markerLength, 0.0f));                // Bottom left
    }

    return pointSet;
}

cv::Size TargetDetector::getGridSize() const
{
    return cv::Size(cols, rows);
}

float TargetDetector::getMarkerLength() const
{
    return markerLength;
}

float TargetDetector::getMarkerSep() const
{
    return markerSeparation;
}