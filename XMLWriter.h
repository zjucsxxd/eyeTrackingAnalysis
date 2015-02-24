/* 
 * File:   XMLWriter.h
 * Author: chili
 *
 * Created on February 10, 2014, 9:19 AM
 */

#ifndef XMLWRITER_H
#define	XMLWRITER_H
#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>



using namespace std;

class XMLWriter {
public:
    XMLWriter();
    XMLWriter(const XMLWriter& orig);
    bool openFile(std::string filename, int nAOI);
    void writeNewAOI(std::vector < std::vector < cv::Mat >> AOIsOnImage,long frameId);
    virtual ~XMLWriter();
    void close();
private:
    std::vector<fstream*> outFiles;
    string filename;
};

#endif	/* XMLWRITER_H */

