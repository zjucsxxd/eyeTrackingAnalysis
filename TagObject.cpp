/* 
 * File:   TagObject.cpp
 * Author: chili
 * 
 * Created on February 18, 2014, 12:09 PM
 */

#include "TagObject.h"

TagObject::TagObject(std::string name, std::vector<int> tags) {
    this->name=name;
    this->tags=tags;
}

TagObject::TagObject(const TagObject& orig) {
    this->name=orig.name;
    this->tags=orig.tags;
}

TagObject::~TagObject() {
}

