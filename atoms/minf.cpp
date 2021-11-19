#include "minf.hpp"
#include "vmhd.hpp"
#include "dinf.hpp"
#include "stbl.hpp"
#include <iostream>

MP4::minf::minf(atomParse& parse)
    : atom(parse)
{
}

MP4::minf::minf(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    if ( build->getComponentSubType() == "vide" ) {
        build->parentPath = path_ + "/";
        child = std::make_shared<vmhd>(build);
        children_.push_back(child);
    }
    
    build->parentPath = path_ + "/";
    child = std::make_shared<dinf>(build);
    children_.push_back(child);
    
    build->parentPath = path_ + "/";
    child = std::make_shared<stbl>(build);
    children_.push_back(child);
}

void MP4::minf::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Information Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::minf::getKey() const
{
    return key;
}

const std::string MP4::minf::key = "minf";

