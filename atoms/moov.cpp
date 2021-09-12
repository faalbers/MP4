#include "moov.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <iomanip>
#include <set>
#include <map>

MP4::moov::moov(internal::atomBuildType &atomBuild, uint64_t filePos)
    : atom(atomBuild, filePos)
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
/*
void MP4::moov::writeAtomChildrenToFile(std::ofstream &fileWrite, char *data)
{
    if ( data == nullptr ) {
        writeAtomChildrenToFile_(fileWrite, data);
        return;
    }

    auto writeInfo = (internal::writeInfoType *) data;

    for ( auto child : children_ ) {
        if ( child->key == "trak" ) {
            // only write track if included in info
            auto track = (trak *) child.get();
            auto trackID = track->getID();
            std::set<uint32_t>::iterator it = writeInfo->excludeTrackIDs.find(trackID);
            if( it != writeInfo->excludeTrackIDs.end() ) continue;
            writeInfo->currentTrackID = trackID;
            child->writeToFile(fileWrite, data);
            continue;
        }
        child->writeToFile(fileWrite, data);
    }
}
*/
std::string MP4::moov::key = "moov";

