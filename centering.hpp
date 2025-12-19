#ifndef CENTERING_HPP
#define CENTERING_HPP

#include <opencv2/opencv.hpp>
#include <cmath>

double calculateCentering(const cv::Mat &warpImg, const cv::Mat &templateImg);

#endif // CENTERING_HPP