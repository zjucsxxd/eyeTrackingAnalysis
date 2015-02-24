/* 
 * File:   VideoAnalisis.cpp
 * Author: chili
 * 
 * Created on February 7, 2014, 10:48 AM
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui_c.h>

#include "VideoAnalysis.h"
#include "Model.h"
#include "TagObject.h"

VideoAnalysis::VideoAnalysis(std::string videoFile, std::string fixationFile) {
    //    int xRes = 640;
    //    int yRes = 480;
    videoName = videoFile;
    // The source of input images
    capture.open(videoFile);
    if (!capture.isOpened()) {
        std::cerr << "Unable to initialise video capture." << std::endl;
    }

    this->fixationFile = new std::ifstream();
    this->fixationFile->open(fixationFile);
    if (!this->fixationFile->is_open())
        std::cerr << "Unable to open fixation file" << std::endl;

    fix2AOIs = new std::ofstream();
    std::stringstream name;
    name << videoFile << ".fix2AOIs.csv";
    fix2AOIs->open(name.str());
    if (!fix2AOIs->is_open())
        std::cerr << "Unable open output file" << std::endl;

    *fix2AOIs << "Shape"
            << "," << "AOI Name"
            << "," << "Start Time"
            << "," << "Duration"
            << "," << "End Time"
            << "," << "X(orig)"
            << "," << "Y(orig)"
            << "," << "Bad Flag"
            << "," << "Comment"
            << "," << "Focus List" << "\n";


    //#ifdef OPENCV3
    //    capture.set(cv::CAP_PROP_FRAME_WIDTH, xRes);
    //    capture.set(cv::CAP_PROP_FRAME_HEIGHT, yRes);
    //#else
    //    capture.set(CV_CAP_PROP_FRAME_WIDTH, xRes);
    //    capture.set(CV_CAP_PROP_FRAME_HEIGHT, yRes);
    //#endif

}

VideoAnalysis::VideoAnalysis(const VideoAnalysis& orig) {
}

VideoAnalysis::~VideoAnalysis() {
}

void VideoAnalysis::run(std::vector<Model> models, std::vector<TagObject> tagObjects) {
    
}

void VideoAnalysis::run2(std::vector<Model> models, std::vector<TagObject> tagObjects) {
    chilitags::Chilitags chilitags;
    int discardedFix = 0;
    bool badFix = false;
    cv::Mat inputImage, outputImage;
    bool exitFlag = false;

    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
            cv::Size(2 * 3 + 1, 2 * 3 + 1),
            cv::Point(-1, -1));

    // The detection is not perfect, so if a tag is not detected during one frame,
    // the tag will shortly disappears, which results in flickering.
    // To address this, Chilitags "cheats" by keeping tags for n frames
    // at the same position. When tags disappear for more than 5 frames,
    // Chilitags actually removes it.
    // Here, we cancel this to show the raw detection results.
    chilitags.setFilter(0, 0.);


    std::map<std::string, cv::Mat> modelImages;

    if (!silentMode) {
        for (Model model : models) {
            cv::namedWindow(model.getName());
        }
        cv::namedWindow("video");
    }

    std::string line;
    getline(*fixationFile, line);
    int count = 1;
    double progress = 0;
    while (getline(*fixationFile, line)) {
        if (capture.get(CV_CAP_PROP_POS_AVI_RATIO) - progress > 0.10) {
            progress = capture.get(CV_CAP_PROP_POS_AVI_RATIO);
            std::cout << progress << std::endl;
        }
        count++;


        std::vector <std::string> fields;
        boost::split(fields, line, boost::is_any_of(","));
        double startTime = boost::lexical_cast<double>(fields[11]);
        double endTime = boost::lexical_cast<double>(fields[13]);
        double duration = boost::lexical_cast<double>(fields[12]);
        double xPos = boost::lexical_cast<double>(fields[14]);
        double yPos = boost::lexical_cast<double>(fields[15]);
        std::string shape = fields[5];
        const cv::Mat gaze = (cv::Mat_<double>(3, 1) << xPos, yPos, 1);

        capture.set(CV_CAP_PROP_POS_MSEC, startTime);

        std::string focus = "";
        std::string tmpFocus = "";
        std::string comment = "";
        std::set<std::string> focusList;
        badFix = false;

        while (capture.get(CV_CAP_PROP_POS_MSEC) <= endTime) {
            int focusCount = 0;
            std::string tmpFocus = "";
            capture.read(inputImage);

            if (!silentMode) {
                outputImage = inputImage.clone();
                cv::circle(outputImage, cv::Point(gaze.at<double>(0, 0), gaze.at<double>(1, 0)), 20, cv::Scalar(255, 0, 0), -1);
            }

            std::map<int, chilitags::Quad> tags = chilitags.find(inputImage);


            /*CODE FOR OBJECT DETECTION*/

            std::vector<std::vector<cv::Point> > contours;
            cv::Mat frame = inputImage.clone();
            cv::Mat hsvFrame, threshold;
            cv::blur(frame, frame, cv::Size(5, 5));
            cv::cvtColor(frame, hsvFrame, CV_RGB2HSV);
            cv::inRange(hsvFrame, lb, ub, threshold);
            cv::erode(threshold, threshold, element);
            cv::erode(threshold, threshold, element);
            cv::erode(threshold, threshold, element);
            cv::dilate(threshold, threshold, element);
            cv::dilate(threshold, threshold, element);
            cv::dilate(threshold, threshold, element);
            cv::dilate(threshold, threshold, element);
            cv::findContours(threshold, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
            for (int i = 0; i < contours.size(); i++) {
                if (!silentMode && contours[i].size() > 5)
                    cv::ellipse(outputImage, cv::fitEllipse(cv::Mat(contours[i])), cv::Scalar(255, 255, 0), 2, 8);
                if (cv::norm(cv::Mat(cv::minAreaRect(cv::Mat(contours[i])).center), cv::Mat(cv::Point2f(xPos, yPos))) < 100)
                    comment = "Blu blob";
            }


            for (TagObject obj : tagObjects) {
                std::string tmp = getObj(obj, tags, cv::Mat(cv::Point2f(xPos, yPos)), contours);
                focusList.insert(tmp);
                if (tmp != "") {
                    tmpFocus = tmp;
                    focusCount++;
                }
            }

            if (tmpFocus == "") {
                for (Model model : models) {
                    std::string tmp = getAOI(model, tags, gaze, &modelImages);
                    if (tmp != "") {
                        tmpFocus = tmp;
                        focusCount++;
                    }
                    focusList.insert(tmp);

                }
            }

            if (tmpFocus == "") {
                tmpFocus = "out";
                focusList.insert("out");
            }

            if (focus == "" && focusCount <= 1) {
                focus = tmpFocus;
            }

            if (focusCount > 1)
                badFix = true;
            if (focus != "" && focus != tmpFocus)
                badFix = true;

            if (!silentMode) {
                std::stringstream txt;
                txt << "AOI:" << focus << " Comment:" << comment;
                cv::putText(outputImage, txt.str(), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0));
                cv::imshow("video", outputImage);
                for (Model model : models)
                    if (modelImages.count(model.getName()) != 0)
                        cv::imshow(model.getName(), modelImages.at(model.getName()));
                if (cv::waitKey(1) == 'q') {
                    exitFlag = true;
                    break;
                }
            }
        }
        if (comment.find("Blu") != std::string::npos && focus.find("Shape") == std::string::npos) {
            cv::Mat tmpImage = inputImage.clone();
            std::stringstream txt;
            txt << count;
            cv::putText(tmpImage, txt.str(), cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0),3);
            cv::circle(tmpImage, cv::Point(gaze.at<double>(0, 0), gaze.at<double>(1, 0)), 20, cv::Scalar(255, 0, 0), -1);
            std::stringstream fileName;
            fileName << videoName << "." << count << ".jpg";
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(30);
            cv::imwrite(fileName.str(), tmpImage,compression_params);
        }
        *fix2AOIs << shape
                << "," << focus
                << "," << startTime
                << "," << duration
                << "," << endTime
                << "," << xPos
                << "," << yPos
                << "," << badFix
                << "," << comment << ",";
        for (std::string s : focusList)
            *fix2AOIs << s << ":";
        *fix2AOIs << "\n";
        //<< "," << gazeOnWs.at<double>(0, 0)
        //<< "," << gazeOnWs.at<double>(1, 0) << "\n";
        if (exitFlag) break;
    }

    std::cout << "Bad Fixation:" << discardedFix;

    if (!silentMode) {
        for (Model model : models)
            if (modelImages.count(model.getName()) != 0)
                cv::destroyWindow(model.getName());
        cv::destroyWindow("video");
    }

    capture.release();
    fix2AOIs->close();
    fixationFile->close();
}

std::string VideoAnalysis::getAOI(Model model, const std::map<int, chilitags::Quad> tags, const cv::Mat gaze, std::map<std::string, cv::Mat>* modelImages) {
    std::vector<cv::Point2f> src, dst;
    for (const std::pair<int, chilitags::Quad> &tag : tags) {
        int id = tag.first;
        if (model.getTagPositions().count(id) == 0) continue;
        // We wrap the corner matrix into a datastructure that allows an
        // easy access to the coordinates
        dst.push_back(cv::Point2f(tag.second(0, 0), tag.second(0, 1)));
        dst.push_back(cv::Point2f(tag.second(1, 0), tag.second(1, 1)));
        dst.push_back(cv::Point2f(tag.second(2, 0), tag.second(2, 1)));
        dst.push_back(cv::Point2f(tag.second(3, 0), tag.second(3, 1)));

        src.push_back(cv::Point2f(model.getTagPositions().at(id)(0, 0), model.getTagPositions().at(id)(0, 1)));
        src.push_back(cv::Point2f(model.getTagPositions().at(id)(1, 0), model.getTagPositions().at(id)(1, 1)));
        src.push_back(cv::Point2f(model.getTagPositions().at(id)(2, 0), model.getTagPositions().at(id)(2, 1)));
        src.push_back(cv::Point2f(model.getTagPositions().at(id)(3, 0), model.getTagPositions().at(id)(3, 1)));
    }
    if (src.size() > 3) {
        cv::Mat H = cv::findHomography(dst, src, CV_RANSAC);
        cv::Mat gazeOnWs = H*gaze;
        gazeOnWs.at<double>(0, 0) /= gazeOnWs.at<double>(2, 0);
        gazeOnWs.at<double>(1, 0) /= gazeOnWs.at<double>(2, 0);
        gazeOnWs.at<double>(2, 0) /= gazeOnWs.at<double>(2, 0);
        if (!silentMode) {
            cv::Mat tmp;
            cv::cvtColor(model.getImage().clone(), tmp, CV_GRAY2RGB);
            cv::circle(tmp, cv::Point(gazeOnWs.at<double>(0, 0), gazeOnWs.at<double>(1, 0)), 20, cv::Scalar(255, 0, 0), -1);
            (*modelImages)[model.getName()] = tmp;
        }

        for (AOI aoi : model.getAOIs()) {
            if (aoi.contains(gazeOnWs.at<double>(0, 0), gazeOnWs.at<double>(1, 0))) {
                return aoi.getName();
            }
        }
        return "";

    } else {

        return "";

    }
}

std::string VideoAnalysis::getObj(TagObject obj, const std::map<int, chilitags::Quad> tags, const cv::Mat gaze, std::vector<std::vector<cv::Point> > contours) {
    /*The gaze is close to the tag*/
    for (int objTag : obj.getTags()) {
        cv::Point2f center;
        if (tags.count(objTag) != 0) {
            const cv::Mat_<cv::Point2f> corners(tags.at(objTag));
            center = 0.5 * (corners(0) + corners(2));
            if (cv::norm(cv::Mat(center), gaze) < 100)
                return obj.getName();
        }
    }
    /*The gaze is close to the blob*/
    std::vector<cv::RotatedRect> blobs;
    for (int objTag : obj.getTags()) {
        cv::Point2f center;
        if (tags.count(objTag) != 0) {
            for (int i = 0; i < contours.size(); i++) {
                const cv::Mat_<cv::Point2f> corners(tags.at(objTag));
                center = 0.5 * (corners(0) + corners(2));
                if (contours[i].size() > 5 && cv::norm(cv::Mat(center), cv::Mat(cv::minAreaRect(cv::Mat(contours[i])).center)) < 100) {
                    blobs.push_back(cv::fitEllipse(cv::Mat(contours[i])));
                }
            }
        }
    }
    for (int i = 0; i < blobs.size(); i++) {
        if (cv::norm(cv::Mat(blobs[i].center), gaze) < 100)
            return obj.getName();
    }
    return "";

}