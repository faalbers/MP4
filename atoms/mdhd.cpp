#include "mdhd.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <chrono>

MP4::mdhd::mdhd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mdhd atom can not parse file: "+filePath_);
    datablock::mdhdDataBlock mdhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &mdhdData, sizeof(mdhdData));
    creationTime = XXH_swap32(mdhdData.creationTime);
    modificationTime = XXH_swap32(mdhdData.modificationTime);
    timeScale = XXH_swap32(mdhdData.timeScale);
    duration = XXH_swap32(mdhdData.duration);
    fileStream.close();
}

void MP4::mdhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "timeScale  : " << timeScale << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
}

void MP4::mdhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::mdhd::key = "mdhd";

