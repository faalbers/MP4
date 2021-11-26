#include "edts.hpp"
#include "elst.hpp"
#include <iostream>

MP4::edts::edts(atomParse& parse)
    : atom(parse)
{
}

MP4::edts::edts(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<elst>(build);
    children_.push_back(child);
}

void MP4::edts::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Edit Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::edts::getKey() const
{
    return key;
}

const std::string MP4::edts::key = "edts";

