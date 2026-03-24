/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Header file for the Renderer (used for rendering virtual objects onto the scene)
 */

#pragma once
#include "config.hpp"
#include <opencv2/opencv.hpp>

class Renderer
{
public:
    // Constructor
    Renderer();

    // Draw a wireframe crystal near the origin (as a floating virtual object)
    void drawCrystal(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                     const cv::Mat &cameraMatrix, const cv::Mat &distCoef);

    // Draw 3D axes at the origin
    void drawAxes(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                  const cv::Mat &cameraMatrix, const cv::Mat &distCoef, float axisLength);

    // Load an .obj file and parse it into vertices and surface normals
    bool loadOBJ(const std::string &path);

    // Draw the imported virtual object
    void drawObject(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                    const cv::Mat &cameraMatrix, const cv::Mat &distCoef);

private:
    std::vector<cv::Point3f> modelVertices;
    std::vector<cv::Point3f> modelNormals;
};