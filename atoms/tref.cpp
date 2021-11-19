#include "tref.hpp"
#include <iostream>

MP4::tref::tref(atomParse& parse)
    : atom(parse)
{
}

MP4::tref::tref(std::shared_ptr<atomBuild> build)
    : atom(build)
{
}


void MP4::tref::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Reference Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::tref::getKey()
{
    return key;
}

std::string MP4::tref::key = "tref";

