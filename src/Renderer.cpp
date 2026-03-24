/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Using the 3D points defined by the TargetDetector, calibrates the camera
 */

#include <fstream>
#include <sstream>
#include "Renderer.hpp"

// Constructor
Renderer::Renderer() {}

struct Face
{
    std::vector<cv::Point> pts; // The 2D screen coordinates
    double depth;               // The Z-distance from the camera
    cv::Scalar color;           // The calculated shaded color
};

// Draw a wireframe crystal near the origin (as a floating virtual object)
void Renderer::drawCrystal(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                           const cv::Mat &cameraMatrix, const cv::Mat &distCoef)
{
    float L = Config::MARKER_LENGTH;

    // Define the 6 vertices of the wireframe crystal shape
    std::vector<cv::Point3f> object3D;
    object3D.push_back(cv::Point3f(2.3f * L, 2.3f * L, -3.0f * L)); // Top point
    object3D.push_back(cv::Point3f(2.3f * L, 2.3f * L, -0.5f * L)); // Bottom point
    object3D.push_back(cv::Point3f(1.6f * L, 2.3f * L, -2.0f * L)); // Mid Left
    object3D.push_back(cv::Point3f(3.0f * L, 2.3f * L, -2.0f * L)); // Mid Right
    object3D.push_back(cv::Point3f(2.3f * L, 1.6f * L, -2.0f * L)); // Mid Front
    object3D.push_back(cv::Point3f(2.3f * L, 3.0f * L, -2.0f * L)); // Mid Back

    // Project all 6 points from 3D space to 2D screen
    std::vector<cv::Point2f> proj2D;
    cv::projectPoints(object3D, rvec, tvec, cameraMatrix, distCoef, proj2D);

    // Draw the lines connecting the points
    // top half (Cyan color)
    cv::line(dst, proj2D[0], proj2D[2], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[0], proj2D[3], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[0], proj2D[4], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[0], proj2D[5], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    // bottom half (Magenta color)
    cv::line(dst, proj2D[1], proj2D[2], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[1], proj2D[3], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[1], proj2D[4], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[1], proj2D[5], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    // middle ring (Yellow color)
    cv::line(dst, proj2D[2], proj2D[4], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[4], proj2D[3], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[3], proj2D[5], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[5], proj2D[2], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
}

void Renderer::drawAxes(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                        const cv::Mat &cameraMatrix, const cv::Mat &distCoef, float axisLength)
{
    cv::drawFrameAxes(dst, cameraMatrix, distCoef, rvec, tvec, axisLength);
}

