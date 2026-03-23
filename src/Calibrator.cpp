/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Using the 3D points defined by the TargetDetector, calibrates the camera
 */

#include "Calibrator.hpp"

// Constructor
Calibrator::Calibrator() {}

std::vector<std::vector<cv::Vec3f>> Calibrator::getPointList() const
{
    return pointList;
}


std::vector<std::vector<cv::Point2f>> Calibrator::getCornerList() const
{
    return cornerList;
}

void Calibrator::addCornersPoints(const std::vector<cv::Point2f>& cornerSet, const std::vector<cv::Vec3f>& pointSet)
{
    cornerList.push_back(cornerSet);
    pointList.push_back(pointSet);
}