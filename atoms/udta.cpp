#include "udta.hpp"
#include "FIRM.hpp"
#include "LENS.hpp"
#include "CAME.hpp"
#include "SETT.hpp"
#include "MUID.hpp"
#include "HMMT.hpp"
#include "BCID.hpp"
#include "GUMI.hpp"
#include "GPMF.hpp"
#include <iostream>

MP4::udta::udta(atomParse& parse)
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

    if ( build->getUserData("LENS") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<LENS>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("CAME") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<CAME>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("SETT") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<SETT>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("MUID") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<MUID>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("HMMT") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<HMMT>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("BCID") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<BCID>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("GUMI") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<GUMI>(build);
        children_.push_back(child);
    }
    
    if ( build->getUserData("GPMF") != "") {
        build->parentPath = path_ + "/";
        child = std::make_shared<GPMF>(build);
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

std::string MP4::udta::getKey() const
{
    return key;
}

const std::string MP4::udta::key = "udta";

