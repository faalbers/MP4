#include "tref.hpp"
#include "tmcd.hpp"
#include <iostream>

MP4::tref::tref(atomParse& parse)
    : atom(parse)
{
}

MP4::tref::tref(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    if ( build->getReferenceTrackIDs("tmcd").size() > 0 ) {
        build->setParentPath(parentPath_ + getKey() + "/");
        child = std::make_shared<tmcd>(build);
        children_.push_back(child);
    }
}

void MP4::tref::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Reference Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::tref::getKey() const
{
    return key;
}

std::map<uint32_t, std::string> MP4::tref::getReferenceTrackIDs() const
{
    std::map<uint32_t, std::string> refTrackIDs;
    for ( auto child : children_ ) {
        if ( child->getKey() == "tmcd" ) {
            for ( auto refTrackID : ((tmcd*) child.get())->trackIDs )
                refTrackIDs[refTrackID] = "tmcd";
        } else error_("getReferenceTrackIDs: referenced track type not recognized: "+child->getKey());
    }
    return refTrackIDs;
}

const std::string MP4::tref::key = "tref";

