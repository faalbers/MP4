#include "trak.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::trak::trak(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
}

void MP4::trak::printData()
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::trak::printHierarchyData()
{
    printData();
    for ( auto child : children_ ) child->printHierarchyData();
}

std::string MP4::trak::key = "trak";

