/* 
 * File:   AOI.h
 * Author: chili
 *
 * Created on February 13, 2014, 3:55 PM
 */

#ifndef AOI_H
#define	AOI_H
#include <opencv2/core/core.hpp>
#include <iostream>


class AOI {
      friend class Model;
public:
    AOI(std::string name, std::vector<cv::Mat> vertex);
    AOI(const AOI& orig);
    bool contains(double x, double y);
    virtual ~AOI();
    std::string getName(){return name;};
private:
    std::vector<cv::Mat> vertex;
    std::string name;
};

#endif	/* AOI_H */

