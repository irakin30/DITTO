#include "scratch2.hpp"
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
        Image,    // binary image (0 and 255)
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

// cv::Mat houghTransform(cv::Mat Image, int threshold)
// {
//     cv::Mat s ;
//     return s;
// }

void scratch2(const std::string path)
{
    cv::Mat Original = imread(path,cv::IMREAD_UNCHANGED);
    // Read Image and convert into gray scale
    cv::Mat Image = imread(path, cv::IMREAD_GRAYSCALE);
    if (Image.empty())
    {
        std::cout << "Error opening image" << std::endl;
        return;
    }
    //find Gradient Magnitude
    cv::Mat magnitude = gaussianBlurGradientMagnitude(Image, 15);
    // Threshold the Image
    cv::Mat closed = thresholdGradientMagnitude(magnitude);
    //return cv::Mat with min <= area <= max
    closed = minimumAreaSize(closed,100,200);

    
    // Convert to color *only for drawing*
    cv::Mat closedColor;
    cv::cvtColor(closed, closedColor, cv::COLOR_GRAY2BGR);

    std::vector<cv::Vec4i> linesP;                      // will hold the results of the detection
    HoughLinesP(closed, linesP, 1, CV_PI / 180, 0); // runs the actual detection
    // Draw the lines
    for (size_t i = 0; i < linesP.size(); i++)
    {
        cv::Vec4i l = linesP[i];
        cv::line(closedColor, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
    }
    cv::Mat closedColor2 = closedColor;
    cv::cvtColor(closedColor, closedColor, cv::COLOR_BGR2GRAY);
    HoughLinesP(closedColor, linesP, 1, CV_PI / 180, 300);
    for (size_t i = 0; i < linesP.size(); i++)
    {
        cv::Vec4i l = linesP[i];
        cv::line(closedColor2, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }
    imwrite("doubledhough.png", closedColor2);
    cv::Mat magnitude_display;
    cv::normalize(magnitude, magnitude_display, 0, 255, cv::NORM_MINMAX, CV_8U);

    std::vector<std::vector<cv::Mat>> imgArr = {{Original, magnitude_display,closed},{Original, closedColor, closedColor2}};

    cv::namedWindow("card", cv::WINDOW_NORMAL);
    cv::resizeWindow("card", 1920, 1080);
    cv::Mat pipeline = displayImage(imgArr);
    cv::imshow("card", pipeline);
    cv::waitKey(0);

    imwrite("Image.png", Image);
    imwrite("Gradient.png", magnitude_display);
    imwrite("minimized.png", closed);
}
