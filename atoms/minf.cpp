#include "minf.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::minf::minf(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(atomBuild, filePath, filePos)
{
}

void MP4::minf::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Information Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::minf::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::minf::key = "minf";

