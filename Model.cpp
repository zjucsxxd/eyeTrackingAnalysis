/* 
 * File:   Model.cpp
 * Author: chili
 * 
 * Created on February 7, 2014, 9:25 AM
 */

#include "Model.h"

Model::Model(std::string modelName, std::string imagePath, std::vector<AOI> AOIs) {
    img = cv::imread(imagePath, 0);
    std::cout << imagePath << "\n";
    if (!img.data) // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
    }
    this->name = modelName;
    this->AOIs = AOIs;
}

void Model::init() {
    cv::namedWindow("DisplayChilitags");

    const static cv::Scalar COLOR(255, 0, 0);
    // OpenCv can draw with sub-pixel precision with fixed point coordinates
    static const int SHIFT = 16;
    static const float PRECISION = 1 << SHIFT;
    cv::Mat outputImage = img.clone();
    cv::cvtColor(img, outputImage, CV_GRAY2RGB);

    chilitags::Chilitags detector;
    std::map<int, chilitags::Quad> tags = detector.find(img);
    if (!silentMode) {
        for (const std::pair<int, chilitags::Quad> & tag : tags) {

            int id = tag.first;
            // We wrap the corner matrix into a datastructure that allows an
            // easy access to the coordinates
            const cv::Mat_<cv::Point2f> corners(tag.second);

            // We start by drawing the borders of the tag
            for (size_t i = 0; i < 4; ++i) {
                cv::line(
                        outputImage,
                        PRECISION * corners(i),
                        PRECISION * corners((i + 1) % 4),
                        COLOR, 3, CV_AA, SHIFT);
            }

            // Other points can be computed from the four corners of the Quad.
            // Chilitags are oriented. It means that the points 0,1,2,3 of
            // the Quad coordinates are consistently the top-left, top-right,
            // bottom-right and bottom-left corners.
            // (i.e. clockwise, starting from top-left)
            // Using this, we can compute (an approximation of) the center of
            // tag.
            cv::Point2f center = 0.5 * (corners(0) + corners(2));
            cv::putText(outputImage, cv::format("%d", id), center,
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, COLOR);
        }

        for (AOI aoi : AOIs) {
            for (cv::Mat vertex : aoi.vertex) {
                cv::circle(outputImage, cv::Point(vertex.at<double>(0, 0), vertex.at<double>(1, 0)), 10, cv::Scalar(0, 255, 0));
            }
        }
        cv::imshow("DisplayChilitags", outputImage);
        cv::waitKey(0);
        cv::destroyWindow("DisplayChilitags");
    }
    tagsPosition = tags;
}

Model::Model(const Model& orig) {
    this->img = orig.img;
    this->name = orig.name;
    this->AOIs = orig.AOIs;
    this->tagsPosition = orig.tagsPosition;
}

Model::~Model() {
}

