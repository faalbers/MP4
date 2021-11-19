#include "stbl.hpp"
#include "stsd.hpp"
#include "stts.hpp"
#include "stsc.hpp"
#include "stsz.hpp"
#include "stss.hpp"
#include "ctts.hpp"
#include "co64.hpp"
#include <iostream>

MP4::stbl::stbl(atomParse& parse)
    : atom(parse)
{
}

MP4::stbl::stbl(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    build->parentPath = path_ + "/";
    child = std::make_shared<stsd>(build);
    children_.push_back(child);

    build->parentPath = path_ + "/";
    child = std::make_shared<stts>(build);
    children_.push_back(child);

    if ( build->samplesHaveSync()) {
        build->parentPath = path_ + "/";
        child = std::make_shared<stss>(build);
        children_.push_back(child);
    }

    if ( build->samplesHaveCompositionOffset()) {
        build->parentPath = path_ + "/";
        child = std::make_shared<ctts>(build);
        children_.push_back(child);
    }

    build->parentPath = path_ + "/";
    child = std::make_shared<stsc>(build);
    children_.push_back(child);

    build->parentPath = path_ + "/";
    child = std::make_shared<stsz>(build);
    children_.push_back(child);

    build->parentPath = path_ + "/";
    child = std::make_shared<co64>(build);
    children_.push_back(child);

}

void MP4::stbl::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Table Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::stbl::getKey()
{
    return key;
}

std::string MP4::stbl::key = "stbl";

