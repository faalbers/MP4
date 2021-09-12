#include "mdia.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::mdia::mdia(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
}

void MP4::mdia::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::mdia::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::mdia::key = "mdia";

