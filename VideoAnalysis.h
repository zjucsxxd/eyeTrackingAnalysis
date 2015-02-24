/* 
 * File:   VideoAnalysis.h
 * Author: chili
 *
 * Created on February 7, 2014, 10:48 AM
 */

#ifndef VIDEOANALYSIS_H
#define	VIDEOANALYSIS_H
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>
#include <set>
#include <chilitags/chilitags.hpp>
#include "AOI.h"
#include "Model.h"
#include "TagObject.h"

extern bool silentMode;
extern std::vector<int> lb;
extern std::vector<int> ub;

class VideoAnalysis {
public:
    VideoAnalysis(std::string videoFile, std::string fixationFile);
    VideoAnalysis(const VideoAnalysis& orig);
    void run(std::vector<Model> models, std::vector<TagObject> tagObjects);
    void run2(std::vector<Model> models, std::vector<TagObject> tagObjects);

    virtual ~VideoAnalysis();
private:
    std::string getAOI(Model model, std::map<int, chilitags::Quad> tags, const cv::Mat gaze, std::map<std::string, cv::Mat>* modelImages);
    std::string getObj(TagObject obj, const std::map<int, chilitags::Quad> tags, const cv::Mat gaze, std::vector<std::vector<cv::Point> > contours);

    cv::VideoCapture capture;
    std::ifstream* fixationFile;
    std::ofstream* fix2AOIs;
    std::string videoName;
};

#endif	/* VIDEOANALISYS_H */

