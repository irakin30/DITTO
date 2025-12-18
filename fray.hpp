#ifndef FRAY_HPP
#define FRAY_HPP

#include <opencv2/opencv.hpp>

int calculateFrayPixels(const cv::Mat &warpImg, const cv::Mat &templateImg);

#endif // FRAY_HPP