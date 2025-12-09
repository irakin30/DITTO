#include "scratch.hpp"
#include "utils.hpp"

cv::Mat gaussianBlurGradientMagnitude(cv::Mat Image, int size)
{
    // Apply Blur
    cv::GaussianBlur(Image, Image, cv::Size(size, size), 1);

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
    float thresh = 0.2f * static_cast<float>(maxVal);
    cv::threshold(Image, Image, thresh, 255, cv::THRESH_BINARY);

    // Not sure what this does GPTED
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
    // GPTED IDK Nearest neighbor
    cv::Mat labels, stats, centroids;

    // Connected Component Labeling (8-connectivity is the classical default)
    int numLabels = cv::connectedComponentsWithStats(
        Image,     // binary image (0 and 255)
        labels,    // output label map
        stats,     // blob statistics
        centroids, // blob centroids
        8          // 8-connectivity
    );

    // Create the cleaned binary image
    cv::Mat cleaned = cv::Mat::zeros(Image.size(), CV_8UC1);

    // Loop over each component (starting at index 1; index 0 is background)
    for (int i = 1; i < numLabels; i++)
    {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);

        if (area >= min && area <= max)
        {
            // keep this component
            cleaned.setTo(255, labels == i);
        }
    }
    //

    return cleaned;
}

cv::Mat houghTransform(cv::Mat Image, int threshold)
{
    cv::Mat clone;
    if (Image.channels() == 3)
    {
        cv::cvtColor(Image, clone, cv::COLOR_BGR2GRAY);
    }
    else if (Image.channels() == 4)
    {
        cv::cvtColor(Image, clone, cv::COLOR_BGRA2GRAY);
    }
    else
    {
        // already grayscale
        clone = Image.clone();
    }

    // Convert to color *only for drawing*
    cv::Mat Colored_Image;
    cv::cvtColor(clone, Colored_Image, cv::COLOR_GRAY2BGR);

    std::vector<cv::Vec4i> linesP;                         // will hold the results of the detection
    HoughLinesP(clone, linesP, 1, CV_PI / 180, threshold); // runs the actual detection
    // Draw the lines
    for (size_t i = 0; i < linesP.size(); i++)
    {
        cv::Vec4i l = linesP[i];
        cv::line(Colored_Image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
    }
    return Colored_Image;
}

void scratch(std::string path)
{
    cv::Mat Image = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (Image.empty())
    {
        std::cerr << "Error: could not load image.\n";
        return;
    }
    // Convert the clone into Grayscaled Image
    cv::Mat grayscaledClone;
    cv::cvtColor(Image, grayscaledClone, cv::COLOR_BGR2GRAY);

    // Smooth the grayscaledClone and find Gradient Magnitude
    cv::Mat magnitude = gaussianBlurGradientMagnitude(grayscaledClone, 9);
    // Threshold the Gradient Magnitude
    cv::Mat closed = thresholdGradientMagnitude(magnitude);
    // return cv::Mat with min <= area <= max
    closed = minimumAreaSize(closed, 50, 500);
    // closed = minimumAreaSize(closed, 400, 500);
    cv::Mat closedColor = houghTransform(closed, 10);
    cv::Mat closedColor2 = houghTransform(closedColor, 10);

    // imwrite("doubledhough.png", closedColor2);
    cv::Mat magnitude_display;
    cv::normalize(magnitude, magnitude_display, 0, 255, cv::NORM_MINMAX, CV_8U);

    std::vector<std::vector<cv::Mat>> imgArr = {{Image, magnitude_display, closed, closedColor, closedColor2}}; //, {Original, closedColor, closedColor2}

    cv::namedWindow("card", cv::WINDOW_NORMAL);
    cv::resizeWindow("card", 1920, 1080);
    cv::Mat pipeline = displayImage(imgArr);
    cv::imshow("card", pipeline);
    cv::waitKey(0);

    imwrite("Image.png", Image);
    imwrite("Gradient.png", magnitude_display);
    imwrite("minimized.png", closed);
}
