#include "udta.hpp"
#include "FIRM.hpp"
#include <iostream>

MP4::udta::udta(atomParse &parse)
    : atom(parse)
{
}

MP4::udta::udta(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    if ( build->getUserData("FIRM") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<FIRM>(build);
        children_.push_back(child);
    }
}

void MP4::udta::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::udta::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::udta::getKey()
{
    return key;
}

std::string MP4::udta::key = "udta";

