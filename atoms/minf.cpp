#include "minf.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::minf::minf(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
}

void MP4::minf::printData()
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Information Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::minf::printHierarchyData()
{
    printData();
    for ( auto child : children_ ) child->printHierarchyData();
}

std::string MP4::minf::key = "minf";

