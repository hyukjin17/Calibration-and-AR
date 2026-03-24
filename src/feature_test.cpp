/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Test function for an ORB feature detector
 * ORB features were used for simplicity (easy OpenCV integration)
 */

#include <opencv2/opencv.hpp>
#include <iostream>

// Global variables for the GUI trackbars
int maxFeatures = 500;
int fastThreshold = 20;

int main()
{
    cv::VideoCapture capdev(0);
    if (!capdev.isOpened())
    {
        std::cerr << "Cannot open camera" << std::endl;
        return -1;
    }

    // Attach UI sliders (trackbars)
    cv::namedWindow("Robust Features", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Max Feat.", "Robust Features", &maxFeatures, 2000);    // max number of features
    cv::createTrackbar("Sensitivity", "Robust Features", &fastThreshold, 100); // feature detection sensitivity

    cv::Mat src, gray, dst;

    for (;;)
    {
        capdev >> src;
        if (src.empty())
            break;

        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

        /**
         * Configure ORB detector using the slider values
         * fastThreshold controls how drastic a contrast change must be to count as a corner
         *
         * inputs: number of features, scale factor, levels in image pyramid, edge threshold (ignore pixels around edge of image)
         *         define first level of pyramid, number of pixels compared, score type, patch size, sensitivity
         */
        cv::Ptr<cv::ORB> detector = cv::ORB::create(std::max(1, maxFeatures), 1.2f, 8, 30,
                                                    0, 2, cv::ORB::HARRIS_SCORE, 30, std::max(1, fastThreshold));

        // Detect keypoints
        std::vector<cv::KeyPoint> keypoints;
        detector->detect(gray, keypoints);
        // Draw detected features onto the frame
        // DRAW_RICH_KEYPOINTS draws circles showing size and orientation of the feature / DEFAULT draws single circle on feature
        cv::drawKeypoints(src, keypoints, dst, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DEFAULT);

        cv::imshow("Robust Features", dst);

        if (cv::waitKey(1) == 'q')
            break;
    }

    return 0;
}