#include "tref.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::tref::tref(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
}

void MP4::tref::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Reference Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::tref::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::tref::key = "tref";

