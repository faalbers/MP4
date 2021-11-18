#include "gmhd.hpp"
#include <iostream>

MP4::gmhd::gmhd(atomParse& parse)
    : atom(parse)
{
}

void MP4::gmhd::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Base Media Information Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::gmhd::printHierarchyData(bool fullLists) const
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::gmhd::getKey()
{
    return key;
}

std::string MP4::gmhd::key = "gmhd";

