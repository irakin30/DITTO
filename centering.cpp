#include "centering.hpp"
#include <opencv2/opencv.hpp>

double calculateCentering(const cv::Mat &warpImg, const cv::Mat &templateImg)
{
    // Convert to grayscale
    cv::Mat grayWarp, grayTemplate;
    if (warpImg.channels() == 3)
    {
        cv::cvtColor(warpImg, grayWarp, cv::COLOR_BGR2GRAY);
    }
    else
    {
        grayWarp = warpImg.clone();
    }

    if (templateImg.channels() == 3)
    {
        cv::cvtColor(templateImg, grayTemplate, cv::COLOR_BGR2GRAY);
    }
    else
    {
        grayTemplate = templateImg.clone();
    }

    cv::Mat result;
    cv::matchTemplate(grayWarp, grayTemplate, result, cv::TM_CCOEFF_NORMED);

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // Convert correlation to deduction (0 = perfect, 1 = worst)
    double templateDeduction = 1.0 - maxVal;

    cv::Mat binary;
    cv::threshold(grayWarp, binary, 50, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double centerDeduction = 1.0;

    if (!contours.empty())
    {
        size_t largestIdx = 0;
        double largestArea = 0;
        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if (area > largestArea)
            {
                largestArea = area;
                largestIdx = i;
            }
        }

        cv::Rect cardRect = cv::boundingRect(contours[largestIdx]);
        cv::Point cardCenter(cardRect.x + cardRect.width / 2, cardRect.y + cardRect.height / 2);
        cv::Point imageCenter(grayWarp.cols / 2, grayWarp.rows / 2);

        // Calculate normalized offset (0 to 1)
        double dx = std::abs(cardCenter.x - imageCenter.x) * 2.0 / grayWarp.cols;
        double dy = std::abs(cardCenter.y - imageCenter.y) * 2.0 / grayWarp.rows;

        // Combine offsets with distance
        centerDeduction = std::sqrt(dx * dx + dy * dy) / std::sqrt(2.0); // Normalize to 0-1
    }

    cv::Mat edgesWarp, edgesTemplate;
    cv::Canny(grayWarp, edgesWarp, 50, 150);
    cv::Canny(grayTemplate, edgesTemplate, 50, 150);

    // Calculate edge similarity
    cv::Mat edgeDiff;
    cv::absdiff(edgesWarp, edgesTemplate, edgeDiff);
    double edgeDiffScore = cv::countNonZero(edgeDiff) /
                           static_cast<double>(edgesWarp.total());
    double edgeDeduction = edgeDiffScore; // 0 to 1

    const double WEIGHT_TEMPLATE = 0.4; // Overall similarity
    const double WEIGHT_CENTER = 0.4;   // Center offset
    const double WEIGHT_EDGE = 0.2;     // Edge alignment

    double combinedDeduction = (templateDeduction * WEIGHT_TEMPLATE +
                                centerDeduction * WEIGHT_CENTER +
                                edgeDeduction * WEIGHT_EDGE);

    double centeringScore = combinedDeduction * 2.0;
    centeringScore = std::min(2.0, std::max(0.0, centeringScore));

    return centeringScore;
}