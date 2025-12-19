#include "scratch.hpp"
#include "utils.hpp"

cv::Mat gaussianBlurGradientMagnitude(cv::Mat Image, int size)
{
    // Apply Blur
    cv::GaussianBlur(Image, Image, cv::Size(size, size), 2);

    cv::Mat sobelx, sobely;
    // Apply Sobel for x-derivative
    cv::Sobel(Image, sobelx, CV_64F, 1, 0, 3);
    // Apply Sobel for y-derivative
    cv::Sobel(Image, sobely, CV_64F, 0, 1, 3);
    // Get the gradient magnitude
    cv::Mat magnitude;
    cv::magnitude(sobelx, sobely, magnitude);
    return magnitude;
}

cv::Mat thresholdGradientMagnitude(cv::Mat Image, float i)
{
    double minVal, maxVal;
    cv::minMaxLoc(Image, &minVal, &maxVal);
    float thresh = 0.3f * static_cast<float>(maxVal);
    cv::threshold(Image, Image, thresh, 255, cv::THRESH_BINARY);

    // Create a 3x3 disk (elliptical) structuring element
    cv::Mat se = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

    // Apply closing: dilation then erosion
    cv::Mat closed;
    cv::morphologyEx(Image, closed, cv::MORPH_CLOSE, se);
    closed.convertTo(closed, CV_8U);
    return closed;
}

cv::Mat minimumAreaSize(cv::Mat Image, int min, int max)
{
    cv::Mat gray;

    // Ensure single-channel
    if (Image.channels() == 1)
        gray = Image.clone();
    else
        cv::cvtColor(Image, gray, cv::COLOR_BGR2GRAY);

    cv::Mat labels, stats, centroids;

    int numLabels = cv::connectedComponentsWithStats(
        gray, labels, stats, centroids, 8);

    cv::Mat cleaned = cv::Mat::zeros(gray.size(), CV_8UC1);

    for (int i = 1; i < numLabels; i++)
    {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);

        if (area >= min && area <= max)
        {
            cleaned.setTo(255, labels == i);
        }
    }

    return cleaned;
}

std::pair<cv::Mat, int> houghTransform(cv::Mat Grayscaled, int minLength, int maxLength, cv::Mat original)
{
    if (Grayscaled.empty() || original.empty())
        return std::pair(cv::Mat(),0);

    cv::Mat gray;
    if (Grayscaled.channels() == 3)
        cv::cvtColor(Grayscaled, gray, cv::COLOR_BGR2GRAY);
    else if (Grayscaled.channels() == 4)
        cv::cvtColor(Grayscaled, gray, cv::COLOR_BGRA2GRAY);
    else
        gray = Grayscaled.clone();

    // Detect lines
    std::vector<cv::Vec4i> linesP;
    cv::HoughLinesP(gray, linesP, 1, CV_PI / 180, 30); // threshold can be tuned

    cv::Mat Colored_Image = original.clone();

    for (const auto& l : linesP)
    {
        int x1 = l[0], y1 = l[1], x2 = l[2], y2 = l[3];
        double length = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));

        // Only draw if length is within min and max
        if (length >= minLength && length <= maxLength)
        {
            cv::line(Colored_Image,
                     cv::Point(x1, y1),
                     cv::Point(x2, y2),
                     cv::Scalar(0, 0, 255),
                     3,
                     cv::LINE_AA);
        }
    }

    return std::pair(Colored_Image,linesP.size());
}

std::pair<cv::Mat, int> scratch(const cv::Mat &Image)
{
    if (Image.empty())
    {
        std::cerr << "Error";
        return std::pair(cv::Mat(),0);
    }
    // Convert the clone into Grayscaled Image
    cv::Mat grayscaledClone;
    cv::cvtColor(Image, grayscaledClone, cv::COLOR_BGR2GRAY);

    // Smooth the grayscaledClone and find Gradient Magnitude
    cv::Mat magnitude = gaussianBlurGradientMagnitude(grayscaledClone, 15);
    // Threshold the Gradient Magnitude
    cv::Mat closed = thresholdGradientMagnitude(magnitude, 0.3);
    // return cv::Mat with min <= area <= max
    closed = minimumAreaSize(closed, 0, 300);
    closed = minimumAreaSize(closed, 10, 100);

    return houghTransform(closed, 1, 100, Image);
}
