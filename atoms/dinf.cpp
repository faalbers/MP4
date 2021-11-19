#include "dinf.hpp"
#include <iostream>

MP4::dinf::dinf(atomParse& parse)
    : atom(parse)
{
}

MP4::dinf::dinf(std::shared_ptr<atomBuild> build)
    : atom(build)
{
}

void MP4::dinf::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Information Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::dinf::getKey() const
{
    return key;
}

const std::string MP4::dinf::key = "dinf";

