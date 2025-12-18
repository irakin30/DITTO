#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "utils.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " {input_image} {template_image}" << std::endl;
        return 0;
    }

    const std::string input_image_file(argv[1]);
    const std::string template_image_file(argv[2]);

    // Read input and template images
    cv::Mat userImg = cv::imread("../images/" + input_image_file);
    cv::Mat templateImg = cv::imread("../images/" + template_image_file);

    // Check if images loaded
    if (userImg.empty() || templateImg.empty())
    {
        std::cerr << "Cannot load images" << std::endl;
        return -1;
    }

    int WIDTH_CARD = getWidthCard();
    int HEIGHT_CARD = getHeightCard();

    cv::Mat originalImg = userImg.clone();

    cv::Mat resizedImg;
    cv::resize(userImg, resizedImg, cv::Size(WIDTH_CARD, HEIGHT_CARD));

    cv::Mat gray, blurred, edges, morph;
    cv::cvtColor(resizedImg, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.5);
    cv::Canny(blurred, edges, 50, 150);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::dilate(edges, morph, kernel, cv::Point(-1, -1), 2);
    cv::erode(morph, morph, kernel, cv::Point(-1, -1), 1);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(morph.clone(), contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> corners = reorderCorners(biggestContour(contours));

    std::vector<cv::Point> scaled_corners;
    if (corners.size() == 4)
    {
        float scale_x = (float)originalImg.cols / WIDTH_CARD;
        float scale_y = (float)originalImg.rows / HEIGHT_CARD;

        for (const auto &corner : corners)
        {
            scaled_corners.push_back(cv::Point(
                (int)(corner.x * scale_x),
                (int)(corner.y * scale_y)));
        }
    }

    cv::Mat cornersImg = resizedImg.clone();
    if (corners.size() == 4)
    {
        drawRectangle(cornersImg, corners);
    }

    cv::Mat warpedImg;
    if (scaled_corners.size() == 4)
    {
        warpedImg = warpCard(originalImg, scaled_corners);
    }
    else
    {
        warpedImg = cv::Mat::zeros(HEIGHT_CARD, WIDTH_CARD, CV_8UC3);
    }

    std::vector<std::vector<cv::Mat>> imgArr = {{resizedImg, gray, blurred},
                                                {edges, cornersImg, warpedImg}};

    cv::Mat pipeline = displayImage(imgArr);

    cv::imshow("Card Detection", pipeline);
    cv::imwrite("../../warp.png", warpedImg);
    cv::waitKey(0);

    return 0;
}