#pragma once
#include <opencv2/opencv.hpp>
#include <string>

void scratch2(const std::string path);
cv::Mat gaussianBlurGradientMagnitude(cv::Mat Image,int size=15);
cv::Mat thresholdGradientMagnitude(cv::Mat Image,float i=0.3);
cv::Mat minimumAreaSize(cv::Mat Image, int min, int max);
cv::Mat houghTransform(cv::Mat Image, int threshold);
