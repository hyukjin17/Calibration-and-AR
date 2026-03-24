/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Main entry point for the program
 * Runs the video loop and creates the AR environment from calibration to virtual object placement
 */

#include <opencv2/core/utils/logger.hpp>
#include "Renderer.hpp"
#include "AppController.hpp"

int main(int argc, char *argv[])
{
    cv::Mat src, dst;
    TargetDetector td;
    Calibrator cal;
    Renderer r;
    AppController controller;
    cv::Mat smoothed_tvec, smoothed_rvec;
    double alpha = 0.5;

    // Mute all OpenCV Info and Warning messages
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    // if calibration images exist, load all existing images and calibrate the camera
    int numImages = cal.loadExistingCalImages(Config::CAL_IMG_FOLDER);
    controller.setSavedCount(numImages);
    bool isCalibrated = cal.loadCalibration(Config::CALIBRATION_FILE);

    // load the virtual object file into memory (for rendering later)
    r.loadOBJ(Config::VIRTUAL_OBJECT_FILE);

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

        if (targetDetected && isCalibrated)
        {
            std::vector<cv::Vec3f> pointSet = td.computePointSet();
            std::vector<cv::Point2f> cornerSet = td.getFlattenedCorners();

            // Solve for the current camera position
            cv::Mat rvec, tvec;
            bool success = cv::solvePnP(pointSet, cornerSet, cal.getCameraMatrix(), cal.getDistCoef(), rvec, tvec);

            if (success)
            {
                if (smoothed_tvec.empty())
                {
                    smoothed_tvec = tvec.clone();
                    smoothed_rvec = rvec.clone();
                }
                else
                {
                    // Blend the new raw tracking data with the old smoothed data
                    smoothed_tvec = (alpha * tvec) + ((1.0 - alpha) * smoothed_tvec);
                    smoothed_rvec = (alpha * rvec) + ((1.0 - alpha) * smoothed_rvec);
                }
                // draw a wireframe crystal near the origin (as a floating virtual object)
                if (controller.getShowCrystal())
                    r.drawCrystal(dst, smoothed_rvec, smoothed_tvec, cal.getCameraMatrix(), cal.getDistCoef());
                // draw 3D axes at the origin
                if (controller.getShowAxes())
                    r.drawAxes(dst, smoothed_rvec, smoothed_tvec, cal.getCameraMatrix(), cal.getDistCoef(), Config::AXIS_LENGTH);
                // draw imported virtual object
                if (controller.getShowObject())
                    r.drawObject(dst, smoothed_rvec, smoothed_tvec, cal.getCameraMatrix(), cal.getDistCoef());
            }

            // Print the real-time movement data
            // std::cout << "Translation (tvec): [" << tvec.at<double>(0) << ", "
            //           << tvec.at<double>(1) << ", " << tvec.at<double>(2) << "]" << std::endl;
            // std::cout << "Rotation (rvec): [" << rvec.at<double>(0) << ", "
            //           << rvec.at<double>(1) << ", " << rvec.at<double>(2) << "]" << std::endl;
        }

        cv::imshow("ArUco Detection Test", dst);
        char key = cv::waitKey(1);
        bool quit = controller.handleInput(key, src, dst, cal, td, targetDetected);
        if (quit)
            break;
    }

    return (0);
}