#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include "image_operations.h"
#include <map>
#include <functional>



using ActionFunction = std::function<void(const cv::Mat&, cv::Mat&, int, char**)>;

int main(int argc, char **argv)
{


    if (argc < 3)
    {
        std::cout << "usage:" << argv[0] << " <image path> <action>" << std::endl;
        std::cout << "actions: grey , rotate90 ,blur , rotate180, flip, resize, canny" << std::endl;
        return -1;
    }

    std::string imagePath = argv[1];
    std::string action = argv[2];
    std::string changed_image_path = argv[3];

    if (argv[2] == "--help" || argv[2] == "-h"){        
        std::cout << "usage:" << argv[0] << " <image path> <action>" << std::endl;
        std::cout << "actions: grey , rotate90 ,blur , rotate180, flip, resize, canny" << std::endl;
        return 0;
    }

    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    if (image.empty())
    {
        std::cout << "Error: Could not read image file." << std::endl;
        return -1;
    }

    std::map<std::string, ActionFunction> actionMap = {
        {"grey", [](const cv::Mat& input, cv::Mat& output, int, char**){ applyGrey(input, output); }},
        {"rotate90", [](const cv::Mat& input, cv::Mat& output, int, char**){ applyRotate90(input, output); }},
        {"blur", [](const cv::Mat& input, cv::Mat& output, int, char**){ applyBlur(input, output); }},
        {"rotate180", [](const cv::Mat& input, cv::Mat& output, int, char**){ apply180(input, output); }},
        {"flip", [](const cv::Mat& input, cv::Mat& output, int, char**){ applyFlip(input, output); }},
        {"resize", [](const cv::Mat& input, cv::Mat& output, int argc, char** argv){
            if (argc < 4)
            {
                std::cout << "usage: " << argv[0] << " <image path> resize <scale factor>" << std::endl;
                return;
            }
            double scaleFactor = std::stod(argv[3]);
            applyResize(input, output, scaleFactor);
        }},
        {"canny", [](const cv::Mat& input, cv::Mat& output, int argc, char** argv){
            if (argc < 5)
            {
                std::cout << "usage: " << argv[0] << " <image path> canny <threshold1> <threshold2>" << std::endl;
                return;
            }
            try
            {
                double threshold1 = std::stod(argv[3]);
                double threshold2 = std::stod(argv[4]);
                applyCanny(input, output, threshold1, threshold2);
            }
            catch (const std::invalid_argument &e)
            {
                std::cout << "Error: Invalid threshold values. Please provide numeric values." << std::endl;
                return;
            }
        }}
    };


    cv::Mat changed_image;
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", image);

    auto it = actionMap.find(action);
    if (it != actionMap.end())
    {
        it->second(image, changed_image, argc, argv);
    }
    else
    {
        std::cout << "Error: Unknown action." << std::endl;
        return -1;
    }
    cv::imwrite(changed_image_path, changed_image);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}