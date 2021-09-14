#include "moov.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <iomanip>
#include <set>
#include <map>

MP4::moov::moov(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
}

void MP4::moov::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::moov::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::moov::writeChildren(std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    //writeChildren_(fileWrite, writeInfo);
    //return;

    for ( auto child : children_ ) {
        if ( child->key == "trak" ) {
            // only write track if included in info
            auto track = (trak *) child.get();
            auto trackID = track->getID();
            std::set<uint32_t>::iterator it = writeInfo.includeTrackIDsA.find(trackID);
            if( it == writeInfo.includeTrackIDsA.end() ) continue;
            child->write(fileWrite, writeInfo);
            continue;
        }
        child->write(fileWrite, writeInfo);
    }
}

std::string MP4::moov::key = "moov";

