#include "moov.hpp"
#include <iostream>
#include <iomanip>
#include <set>
#include <map>

MP4::moov::moov(atomBuild &build)
    : atom(build)
{
}

void MP4::moov::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::moov::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::moov::key = "moov";

