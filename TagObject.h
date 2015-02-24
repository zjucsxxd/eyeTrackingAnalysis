/* 
 * File:   TagObject.h
 * Author: chili
 *
 * Created on February 18, 2014, 12:09 PM
 */

#ifndef TAGOBJECT_H
#define	TAGOBJECT_H
#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>

class TagObject {
public:
    TagObject(std::string name, std::vector<int> tags);
    TagObject(const TagObject& orig);
    virtual ~TagObject();
    std::string getName(){return name;};
    const std::vector<int> getTags(){return tags;};
    
private:
    std::string name;
    std::vector<int> tags;

};

#endif	/* TAGOBJECT_H */

