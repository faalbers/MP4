#include "free.hpp"
#include <iostream>

MP4::free::free(atomBuildType &atomBuild)
    : atom(atomBuild)
{
    freeSize = fileNextPos_ - fileDataPos_;
}

void MP4::free::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Free Space Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "freeSize  : " << freeSize << std::endl;
}

void MP4::free::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::free::key = "free";

