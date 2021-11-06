#include "mdia.hpp"
#include "minf.hpp"
#include <iostream>

MP4::mdia::mdia(atomParse &parse)
    : atom(parse)
{
}

MP4::mdia::mdia(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    headerSize_ = 8;
    path_ = parentPath_ + key;

    std::shared_ptr<atom> child;

    build->parentPath = path_ + "/";
    child = std::make_shared<minf>(build);
    children_.push_back(child);

}

void MP4::mdia::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::mdia::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::mdia::getKey()
{
    return key;
}

std::string MP4::mdia::key = "mdia";

