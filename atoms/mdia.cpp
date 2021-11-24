#include "mdia.hpp"
#include "mdhd.hpp"
#include "hdlr.hpp"
#include "minf.hpp"
#include <iostream>

MP4::mdia::mdia(atomParse& parse)
    : atom(parse)
{
}

MP4::mdia::mdia(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<mdhd>(build);
    children_.push_back(child);

    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<hdlr>(build);
    children_.push_back(child);

    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<minf>(build);
    children_.push_back(child);

}

void MP4::mdia::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::mdia::getKey() const
{
    return key;
}

const std::string MP4::mdia::key = "mdia";

