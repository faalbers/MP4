#include "gmhd.hpp"
#include <iostream>

MP4::gmhd::gmhd(atomParse& parse)
    : atom(parse)
{
}

void MP4::gmhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Base Media Information Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::gmhd::getKey()
{
    return key;
}

const std::string MP4::gmhd::key = "gmhd";

