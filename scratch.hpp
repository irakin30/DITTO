#pragma once
#include <opencv2/opencv.hpp>
#include <string>

void scratch(std::string path);
// Apply a sizexsize guassianBlur and finding Gradient Magnitude
cv::Mat gaussianBlurGradientMagnitude(cv::Mat Image,int size=15);
// Threshold Gradient Magnitude by i% of max value
cv::Mat thresholdGradientMagnitude(cv::Mat Image,float i=0.3);
// Threshold for minimum size of a area 
cv::Mat minimumAreaSize(cv::Mat Image, int min, int max);
// Detect and draw lines at a certain threshold vote 
cv::Mat houghTransform(cv::Mat Image, int threshold);
