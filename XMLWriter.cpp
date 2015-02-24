/* 
 * File:   XMLWriter.cpp
 * Author: chili
 * 
 * Created on February 10, 2014, 9:19 AM
 */

#include "XMLWriter.h"

XMLWriter::XMLWriter() {


}

bool XMLWriter::openFile(std::string filename, int nAOI) {
    bool flag = true;
    this->filename = filename;
    for (int i = 0; i < nAOI; i++) {
        fstream* tmp = new fstream();
        stringstream name;
        name << filename << ".AOI" << i << ".xml";
        tmp->open(name.str(),ios_base::in | ios_base::out|ios_base::trunc);
        outFiles.push_back(tmp);
        flag = flag & tmp->is_open();
        if(!flag)
                perror("Error:");
    }
    return flag;
}

XMLWriter::XMLWriter(const XMLWriter& orig) {
}

XMLWriter::~XMLWriter() {
}

void XMLWriter::writeNewAOI(std::vector < std::vector < cv::Mat >> AOIsOnImage, long frameId) {
    for (int i = 0; i < AOIsOnImage.size(); i++) {
        std::vector<cv::Mat> AOI = AOIsOnImage.at(i);
        *outFiles[i] << "<KeyFrame>\n"<<"<Points>\n";
        for (cv::Mat vertex : AOI) {
            *outFiles[i] << "<Point>\n";
            *outFiles[i] << "<X>" << round(vertex.at<double>(0, 0)) << "</X>\n";
            *outFiles[i] << "<Y>" << round(vertex.at<double>(1, 0)) << "</Y>\n";
            *outFiles[i] << "</Point>\n";
        }
        *outFiles[i] << "</Points>\n";
        *outFiles[i] << "<Visible>true</Visible>\n";
        *outFiles[i] << "<Timestamp>" << frameId * 1000 << "</Timestamp>\n";
        *outFiles[i] << "</KeyFrame>\n";
    }
}

void XMLWriter::close() {
    ofstream output;
    stringstream name;
    name << filename << "_AOI" << ".xml";
    output.open(name.str());
        std::cout<<"output file : "<<name.str()<<"\n";


    output << "<?xml version=\"1.0\"?>\n";
    output << "<ArrayOfDynamicAOI xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n";

    for (int i = 0; i < outFiles.size(); i++) {
        outFiles[i]->flush();
        outFiles[i]->seekg(0, outFiles[i]->beg);

        output << "<DynamicAOI>\n"
                << "<ID>" << 100 + i + 1 << "</ID>\n"
                << "<ParentID>1</ParentID>\n"
                << "<Points>\n"
                << "<Point>\n"
                << "<X>1</X>\n"
                << "<Y>1</Y>\n"
                << "</Point>\n"
                << "<Point>\n"
                << "<X>1</X>\n"
                << "<Y>2</Y>\n"
                << "</Point>\n"
                << "<Point>\n"
                << "<X>2</X>\n"
                << "<Y>2</Y>\n"
                << "</Point>\n"
                << "<Point>\n"
                << "<X>2</X>\n"
                << "<Y>1</Y>\n"
                << "</Point>\n"
                << "</Points>\n"
                << "<Type>Polygon</Type>\n"
                << "<Style>HalfTransparent</Style>\n"
                << "<Name>AOI" << i + 1 << "</Name>\n"
                << "<Visible> true </Visible>\n"
                << "<CurrentTimestamp > 0 </CurrentTimestamp>\n"
                << "<KeyFrames>\n";
        string line;
        while (getline(*outFiles[i], line)) {
            output << line << '\n';
        }
        output << "</KeyFrames>\n"
                << "</DynamicAOI>\n";
        outFiles[i]->close();
    }

    output << "</ArrayOfDynamicAOI>\n";

}
