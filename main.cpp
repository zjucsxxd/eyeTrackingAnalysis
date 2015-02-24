/* 
 * File:   main.cpp
 * Author: chili
 *
 * Created on February 7, 2014, 9:21 AM
 */

#include <cstdlib>
#include <iostream>
#include "Model.h"
#include "VideoAnalysis.h"
#include "pugixml.cpp"
#include "TagObject.h"

using namespace std;


bool silentMode = false;
std::vector<int> lb(3);
std::vector<int> ub(3);

/*
 * 
 */
void parseModel(pugi::xml_node, vector<Model> *);
void parseTagObject(pugi::xml_node, vector<TagObject> *);


int main(int argc, char** argv) {
    if (argc < 6) return -1;
    cout << "Usage: -m modelFile.xml -f fixationFile -v videoFile (-s silent mode) -lb lh ls lv -ub uh us uv\n";
    string modelFile, videoFile, fixationFile;
    for (int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
        if (string(argv[i]) == "-m")
            modelFile = string(argv[i + 1]);
        else if (string(argv[i]) == "-f")
            fixationFile = string(argv[i + 1]);
        else if (string(argv[i]) == "-v")
            videoFile = string(argv[i + 1]);
        else if (string(argv[i]) == "-s")
            silentMode = true;
         else if (string(argv[i]) == "-lb"){
             lb[0]=boost::lexical_cast<int>(argv[i+1]);
             lb[1]=boost::lexical_cast<int>(argv[i+2]);
             lb[2]=boost::lexical_cast<int>(argv[i+3]);
         }
         else if (string(argv[i]) == "-ub"){
             ub[0]=boost::lexical_cast<int>(argv[i+1]);
             ub[1]=boost::lexical_cast<int>(argv[i+2]);
             ub[2]=boost::lexical_cast<int>(argv[i+3]);
         }
    }
    cout << modelFile << endl;
    vector<Model> models;
    vector<TagObject> tagObjects;


    pugi::xml_document configFile;
    pugi::xml_parse_result result = configFile.load_file(modelFile.c_str());
    cout << "Result:" << result.description() << "\n";

    for (pugi::xml_node node = configFile.first_child(); node; node = node.next_sibling()) {
        if (string(node.name()) == "Model")
            parseModel(node, &models);
        if (string(node.name()) == "TagObject")
            parseTagObject(node, &tagObjects);
    }
    VideoAnalysis VA(videoFile, fixationFile);
    VA.run2(models,tagObjects);

    return 0;
}

void parseModel(pugi::xml_node modelNode, vector<Model> *models) {

    string modelName = modelNode.attribute("name").as_string();
    string imagePath;
    vector<AOI> AOIs;
    for (pugi::xml_node node = modelNode.first_child(); node; node = node.next_sibling()) {
        if (string(node.name()) == "Image") {
            imagePath = node.text().as_string();
        } else {
            string AOIName = node.attribute("name").as_string();
            std::vector<cv::Mat> vertex;
            for (pugi::xml_node pointNode = node.first_child(); pointNode; pointNode = pointNode.next_sibling()) {
                double x = pointNode.child("X").text().as_double();
                double y = pointNode.child("Y").text().as_double();
                vertex.push_back((cv::Mat_<double>(3, 1) << x, y, 1));
            }
            AOI aoi(AOIName, vertex);
            AOIs.push_back(aoi);
        }
    }
    Model model(modelName, imagePath, AOIs);
    model.init();
    models->push_back(model);
}
void parseTagObject(pugi::xml_node objectNode, vector<TagObject> *objects) {
    vector<int> tags;
    string objectName = objectNode.attribute("name").as_string();
    for (pugi::xml_node node = objectNode.first_child(); node; node = node.next_sibling()) {
        if (string(node.name()) == "Tag") {
            tags.push_back(node.text().as_int());
            }
         
        }
    
    TagObject obj(objectName,tags);
    objects->push_back(obj);
}
