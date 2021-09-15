#include "tmcd.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::tmcd::tmcd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("vmhd atom can not parse file: "+filePath_);
    uint32_t trackID;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    do {
        fileStream.read((char *) &trackID, sizeof(trackID));
        trackIDs.push_back(_byteswap_ulong(trackID));
    } while ( fileStream.tellg() < fileNextPos_ );
    fileStream.close();
}

void MP4::tmcd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Reference Atom)" << std::endl;
    std::cout << dataIndent << "[#] (trackID)\n";
    size_t index = 1;
    for ( auto entry : trackIDs ) {
        std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
        index++;
    }
}

void MP4::tmcd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::tmcd::key = "tmcd";

