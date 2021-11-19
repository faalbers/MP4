#include "free.hpp"
#include <iostream>

MP4::free::free(atomParse& parse)
    : atom(parse)
{
    freeSize = fileNextPos_ - fileDataPos_;
}

void MP4::free::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Free Space Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "freeSize  : " << freeSize << std::endl;
}

std::string MP4::free::getKey() const
{
    return key;
}

const std::string MP4::free::key = "free";

