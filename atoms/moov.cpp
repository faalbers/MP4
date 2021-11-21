#include "moov.hpp"
#include "mvhd.hpp"
#include "udta.hpp"
#include "trak.hpp"
#include <iostream>
#include <iomanip>
#include <set>
#include <map>

MP4::moov::moov(atomParse& parse)
    : atom(parse)
{
}

MP4::moov::moov(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    build->setParentPath(path_ + "/");
    child = std::make_shared<mvhd>(build);
    children_.push_back(child);
    
    build->setParentPath(path_ + "/");
    child = std::make_shared<udta>(build);
    children_.push_back(child);
    
    while ( build->setNextTrack() ) {
        build->setParentPath(path_ + "/");
        child = std::make_shared<trak>(build);
        children_.push_back(child);
    }
}

void MP4::moov::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::moov::getKey() const
{
    return key;
}

const std::string MP4::moov::key = "moov";

