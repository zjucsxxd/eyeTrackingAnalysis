/* 
 * File:   AOI.cpp
 * Author: chili
 * 
 * Created on February 13, 2014, 3:55 PM
 */

#include "AOI.h"

AOI::AOI(std::string name, std::vector<cv::Mat> vertex) {

    this->name = name;
    this->vertex = vertex;
}

AOI::AOI(const AOI& orig) {
    this->name=orig.name;
    this->vertex=orig.vertex;
    
}

AOI::~AOI() {
}

bool AOI::contains(double x, double y) {
    int i, j;
    bool c = false;
    for (i = 0, j = vertex.size() - 1; i < vertex.size(); j = i++) {
        if (((vertex[i].at<double>(1, 0) > y) != (vertex[j].at<double>(1, 0) > y)) &&
                (x < (vertex[j].at<double>(0, 0) - vertex[i].at<double>(0, 0)) * (y - vertex[i].at<double>(1, 0)) / (vertex[j].at<double>(1, 0) - vertex[i].at<double>(1, 0)) + vertex[i].at<double>(0, 0)))
            c = !c;
    }
    return c;
}
