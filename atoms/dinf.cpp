#include "dinf.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::dinf::dinf(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
}

void MP4::dinf::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Information Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::dinf::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::dinf::key = "dinf";

