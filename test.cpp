#include <iostream>
#include "scratch.hpp"
#include "utils.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " {input_image}" << std::endl;
        return 0;
    }

    const std::string input_image_file(argv[1]);
    // cv::Mat image = cv::imread(input_image_file, -1);
    // scratchDetect(input_image_file,300);
    scratch(input_image_file);
}
