/* 
 * File:   Model.h
 * Author: chili
 *
 * Created on February 7, 2014, 9:25 AM
 */

#ifndef Model_H
#define	Model_H

#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <chilitags/chilitags.hpp>
#include <iostream>
#include <cstdlib>

#include "AOI.h"
extern bool silentMode;

class Model {
public:
    Model(std::string modelName, std::string imagePath, std::vector<AOI> AOIs);
    Model(const Model& orig);
    std::map<int, chilitags::Quad> getTagPositions(){return tagsPosition;};
    void init();
    std::vector<AOI> getAOIs(){return AOIs;};
    std::string getName(){return name;};
    cv::Mat getImage(){return img;};
    virtual ~Model();
private:
    cv::Mat img;
    std::string name;
    std::vector<AOI> AOIs;
    std::map<int, chilitags::Quad> tagsPosition;
};

#endif	/* Model_H */

