#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

// Constants for card dimensions
const int WIDTH_CARD = 330;
const int HEIGHT_CARD = 440;

// Get card width
int getWidthCard()
{
    return WIDTH_CARD;
}

// Get card height
int getHeightCard()
{
    return HEIGHT_CARD;
}

// Find the corners and the area of the best contour for card outline
std::vector<cv::Point> biggestContour(const std::vector<std::vector<cv::Point>> &contours)
{
    int bestIdx = -1;
    double bestArea = 0.0;

    for (int i = 0; i < (int)contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area < 5000)
            continue;

        cv::RotatedRect rect = cv::minAreaRect(contours[i]);

        if (area > bestArea)
        {
            bestArea = area;
            bestIdx = i;
        }
    }

    if (bestIdx < 0)
        return {};

    std::vector<cv::Point> hull;
    cv::convexHull(contours[bestIdx], hull);

    if (hull.size() < 4)
        return {};

    double peri = cv::arcLength(hull, true);
    if (peri <= 0.0)
        return {};

    std::vector<cv::Point> approx;
    cv::approxPolyDP(hull, approx, 0.02 * peri, true);

    cv::RotatedRect rect = cv::minAreaRect(hull);
    rect.size.width *= 0.98f;
    rect.size.height *= 0.98f;

    cv::Point2f pts2f[4];
    rect.points(pts2f);

    std::vector<cv::Point> corners(4);
    for (int i = 0; i < 4; i++)
        corners[i] = pts2f[i];

    return corners;
}

std::pair<std::vector<int>, std::vector<int>> sortVals(const std::vector<int> &arr)
{
    std::vector<int> sorted_arr = arr;
    std::vector<int> indices(arr.size());

    for (int i = 0; i < arr.size(); i++)
    {
        indices[i] = i;
    }

    std::sort(indices.begin(), indices.end(), [&arr](int a, int b)
              { return arr[a] < arr[b]; });

    for (int i = 0; i < arr.size(); i++)
    {
        sorted_arr[i] = arr[indices[i]];
    }

    return {sorted_arr, indices};
}

std::vector<cv::Point> reorderCorners(std::vector<cv::Point> corners)
{
    std::vector<int> xvals = {corners[0].x, corners[1].x, corners[2].x, corners[3].x};
    std::vector<int> yvals = {corners[0].y, corners[1].y, corners[2].y, corners[3].y};

    auto [sorted_yvals, y_idxs] = sortVals(yvals);
    yvals = sorted_yvals;

    std::vector<int> xvals_temp = xvals;
    for (int i = 0; i < 4; i++)
    {
        xvals[i] = xvals_temp[y_idxs[i]];
    }

    if (yvals[0] == yvals[1])
    {
        if (xvals[1] < xvals[0])
        {
            std::swap(xvals[0], xvals[1]);
        }
    }

    double dist1 = std::sqrt(std::pow(xvals[1] - xvals[0], 2) + std::pow(yvals[1] - yvals[0], 2));
    double dist2 = std::sqrt(std::pow(xvals[2] - xvals[0], 2) + std::pow(yvals[2] - yvals[0], 2));
    double dist3 = std::sqrt(std::pow(xvals[3] - xvals[0], 2) + std::pow(yvals[3] - yvals[0], 2));
    std::vector<double> dists = {dist1, dist2, dist3};

    std::vector<double> dists_copy = dists;
    std::sort(dists_copy.begin(), dists_copy.end());

    std::vector<int> dist_idxs(3);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (std::abs(dists[j] - dists_copy[i]) < 1e-6)
            {
                dist_idxs[i] = j;
                break;
            }
        }
    }

    dist_idxs.insert(dist_idxs.begin(), 0);
    dist_idxs[1] += 1;
    dist_idxs[2] += 1;
    dist_idxs[3] += 1;

    cv::Point topleft, topright, bottomleft, bottomright;

    if (yvals[0] == yvals[1])
    {
        if (std::abs(dists[0] - dists_copy[0]) < 1e-6)
        { // Card is vertical
            topleft = cv::Point(xvals[dist_idxs[0]], yvals[dist_idxs[0]]);
            topright = cv::Point(xvals[dist_idxs[1]], yvals[dist_idxs[1]]);
            bottomright = cv::Point(xvals[dist_idxs[3]], yvals[dist_idxs[3]]);
            bottomleft = cv::Point(xvals[dist_idxs[2]], yvals[dist_idxs[2]]);
        }
        else
        { // Card is horizontal
            topleft = cv::Point(xvals[dist_idxs[1]], yvals[dist_idxs[1]]);
            topright = cv::Point(xvals[dist_idxs[0]], yvals[dist_idxs[0]]);
            bottomright = cv::Point(xvals[dist_idxs[2]], yvals[dist_idxs[2]]);
            bottomleft = cv::Point(xvals[dist_idxs[3]], yvals[dist_idxs[3]]);
        }
    }
    else
    { // Card is tilted
        int min_x = *std::min_element(xvals.begin(), xvals.end());

        if (xvals[dist_idxs[1]] == min_x)
        {
            topleft = cv::Point(xvals[dist_idxs[1]], yvals[dist_idxs[1]]);
            topright = cv::Point(xvals[dist_idxs[0]], yvals[dist_idxs[0]]);
            bottomright = cv::Point(xvals[dist_idxs[2]], yvals[dist_idxs[2]]);
            bottomleft = cv::Point(xvals[dist_idxs[3]], yvals[dist_idxs[3]]);
        }
        else
        {
            topleft = cv::Point(xvals[dist_idxs[0]], yvals[dist_idxs[0]]);
            topright = cv::Point(xvals[dist_idxs[1]], yvals[dist_idxs[1]]);
            bottomright = cv::Point(xvals[dist_idxs[3]], yvals[dist_idxs[3]]);
            bottomleft = cv::Point(xvals[dist_idxs[2]], yvals[dist_idxs[2]]);
        }
    }

    return {topleft, topright, bottomleft, bottomright};
}

void drawRectangle(cv::Mat &image, const std::vector<cv::Point> &corners)
{
    int thickness = 1;
    cv::line(image, corners[0], corners[1], cv::Scalar(0, 255, 0), thickness); // Top edge
    cv::line(image, corners[0], corners[2], cv::Scalar(0, 255, 0), thickness); // Left edge
    cv::line(image, corners[1], corners[3], cv::Scalar(0, 255, 0), thickness); // Right edge
    cv::line(image, corners[2], corners[3], cv::Scalar(0, 255, 0), thickness); // Bottom edge
}

cv::Mat warpCard(const cv::Mat &image, const std::vector<cv::Point> &corners)
{
    cv::Mat result;

    std::vector<cv::Point> orderedCorners = reorderCorners(corners);

    std::vector<cv::Point2f> srcPoints(4);
    std::vector<cv::Point2f> dstPoints(4);

    for (int i = 0; i < 4; i++)
    {
        srcPoints[i] = cv::Point2f(static_cast<float>(orderedCorners[i].x), static_cast<float>(orderedCorners[i].y));
    }

    dstPoints[0] = cv::Point2f(0.0f, 0.0f);
    dstPoints[1] = cv::Point2f(static_cast<float>(WIDTH_CARD), 0.0f);
    dstPoints[2] = cv::Point2f(0.0f, static_cast<float>(HEIGHT_CARD));
    dstPoints[3] = cv::Point2f(static_cast<float>(WIDTH_CARD), static_cast<float>(HEIGHT_CARD));

    cv::Mat transform = cv::getPerspectiveTransform(srcPoints, dstPoints);
    cv::warpPerspective(image, result, transform, cv::Size(WIDTH_CARD, HEIGHT_CARD));

    return result;
}

cv::Mat displayImage(const std::vector<std::vector<cv::Mat>> &imgArr)
{
    size_t rows = imgArr.size();
    size_t cols = imgArr[0].size();

    std::vector<std::vector<cv::Mat>> processedImgArr = imgArr;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (processedImgArr[i][j].channels() == 1)
            {
                cv::cvtColor(processedImgArr[i][j], processedImgArr[i][j], cv::COLOR_GRAY2BGR);
            }
        }
    }

    std::vector<cv::Mat> row(rows);

    for (int i = 0; i < rows; ++i)
    {
        std::vector<cv::Mat> rowImages = processedImgArr[i];
        cv::hconcat(rowImages, row[i]);
    }

    cv::Mat grid;
    cv::vconcat(row, grid);

    return grid;
}