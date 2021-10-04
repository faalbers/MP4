#include "tmcd.hpp"
#include <iostream>

MP4::tmcd::tmcd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
    , isTrackReference(false)
{
    // there are several atoms with the same key name.
    if (parentPath_.find("/tref") != std::string::npos) {
        isTrackReference = true;
    }

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("vmhd atom can not parse file: "+filePath_);
    fileStream.seekg(fileDataPos_, fileStream.beg);

    if ( isTrackReference ) {
        uint32_t trackID;
        do {
            fileStream.read((char *) &trackID, sizeof(trackID));
            trackIDs.push_back(XXH_swap32(trackID));
        } while ( fileStream.tellg() < fileNextPos_ );
    } else {
    }

    fileStream.close();
}

void MP4::tmcd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    if ( isTrackReference ) {
        std::cout << path_ << " (Track Reference Atom) ["<< headerSize_ << "]" << std::endl;
        std::cout << dataIndent << "[#] (trackID)\n";
        size_t index = 1;
        for ( auto entry : trackIDs ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        std::cout << path_ << " (Timecode Sample Description Atom) ["<< headerSize_ << "]" << std::endl;
        std::cout << dataIndent << "This is a container Atom ..." << std::endl;
    }
}

void MP4::tmcd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::tmcd::key = "tmcd";

