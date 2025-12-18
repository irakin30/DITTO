#include "fray.hpp"
#include <opencv2/opencv.hpp>

int calculateFrayPixels(const cv::Mat &warpImg, const cv::Mat &templateImg)
{
    // Check if images are valid
    if (warpImg.empty() || templateImg.empty())
    {
        return 0;
    }

    // Ensure images have the same size
    cv::Mat processedWarp = warpImg.clone();
    cv::Mat processedTemplate = templateImg.clone();

    if (processedWarp.size() != processedTemplate.size())
    {
        cv::resize(processedWarp, processedWarp, processedTemplate.size());
    }

    // Convert to grayscale if needed
    cv::Mat warpGray, templateGray;
    if (processedWarp.channels() == 3)
    {
        cv::cvtColor(processedWarp, warpGray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        warpGray = processedWarp.clone();
    }

    if (processedTemplate.channels() == 3)
    {
        cv::cvtColor(processedTemplate, templateGray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        templateGray = processedTemplate.clone();
    }

    // Create binary masks (threshold to separate card from background)
    cv::Mat warpMask, templateMask;

    // Use adaptive thresholding for better results with varying lighting
    cv::adaptiveThreshold(warpGray, warpMask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 11, 2);
    cv::adaptiveThreshold(templateGray, templateMask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 11, 2);

    // Clean up masks with morphological operations
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(warpMask, warpMask, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(templateMask, templateMask, cv::MORPH_CLOSE, kernel);

    // Find edges/contours of the template (perfect card)
    std::vector<std::vector<cv::Point>> templateContours;
    cv::findContours(templateMask, templateContours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    if (templateContours.empty())
    {
        return 0; // No template contour found
    }

    // Get the largest contour (should be the card)
    auto largestContour = std::max_element(templateContours.begin(), templateContours.end(),
                                           [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b)
                                           {
                                               return cv::contourArea(a) < cv::contourArea(b);
                                           });

    // Create a mask for the template card shape (filled)
    cv::Mat templateCardMask = cv::Mat::zeros(templateMask.size(), CV_8UC1);
    cv::drawContours(templateCardMask, std::vector<std::vector<cv::Point>>{*largestContour},
                     -1, cv::Scalar(255), cv::FILLED);

    // Create edge mask for the template with specified thickness
    cv::Mat templateEdgeMask = cv::Mat::zeros(templateMask.size(), CV_8UC1);
    cv::drawContours(templateEdgeMask, std::vector<std::vector<cv::Point>>{*largestContour},
                     -1, cv::Scalar(255), 2); // 2 pixel thick edge

    // Create edge mask for the warped image
    cv::Mat warpEdges;
    cv::Canny(warpGray, warpEdges, 50, 150);

    // Dilate edges slightly to account for edge detection variations
    cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(warpEdges, warpEdges, dilateKernel);

    // Find contours in the warped image
    std::vector<std::vector<cv::Point>> warpContours;
    cv::findContours(warpMask, warpContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (warpContours.empty())
    {
        return 0;
    }

    // Get the largest contour from warped image
    auto largestWarpContour = std::max_element(warpContours.begin(), warpContours.end(),
                                               [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b)
                                               {
                                                   return cv::contourArea(a) < cv::contourArea(b);
                                               });

    // Create mask for warped card area
    cv::Mat warpCardMask = cv::Mat::zeros(warpMask.size(), CV_8UC1);
    cv::drawContours(warpCardMask, std::vector<std::vector<cv::Point>>{*largestWarpContour},
                     -1, cv::Scalar(255), cv::FILLED);

    // Calculate fraying pixels:
    // 1. Pixels that are edges in the warped image
    // 2. Within the warped card area
    // 3. Outside the template edge (allowing for some margin)
    // 4. Not within the template card area (to avoid counting interior details)

    int frayCount = 0;

    // Create a slightly dilated template edge for better comparison
    cv::Mat dilatedTemplateEdge;
    cv::dilate(templateEdgeMask, dilatedTemplateEdge, dilateKernel);

    // Also create eroded template card mask to ensure we don't miss fraying near edges
    cv::Mat erodedTemplateCard;
    cv::erode(templateCardMask, erodedTemplateCard, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));

    for (int y = 0; y < warpEdges.rows; y++)
    {
        for (int x = 0; x < warpEdges.cols; x++)
        {
            // Check if pixel is an edge in warped image
            if (warpEdges.at<uchar>(y, x) > 0)
            {
                // Check if pixel is within the warped card area
                if (warpCardMask.at<uchar>(y, x) > 0)
                {
                    // Check if pixel is outside the template edge (with some margin)
                    if (dilatedTemplateEdge.at<uchar>(y, x) == 0)
                    {
                        // Additional check: not too far inside the template card
                        if (erodedTemplateCard.at<uchar>(y, x) == 0)
                        {
                            frayCount++;
                        }
                    }
                }
            }
        }
    }

    return frayCount;
}