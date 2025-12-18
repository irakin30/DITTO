#pragma once
#include <opencv2/opencv.hpp>
#include <string>

std::pair<cv::Mat, int> scratch(const cv::Mat& Image);
// Apply a sizexsize guassianBlur and finding Gradient Magnitude
cv::Mat gaussianBlurGradientMagnitude(cv::Mat Image,int size=15);
// Threshold Gradient Magnitude by i% of max value
cv::Mat thresholdGradientMagnitude(cv::Mat Image,float i=0.3);
// Threshold for minimum size of a area 
cv::Mat minimumAreaSize(cv::Mat Image, int min, int max);
// Detect and draw lines at a certain threshold vote 
std::pair<cv::Mat, int> houghTransform(cv::Mat Grayscaled, int min, int max, cv::Mat original);
