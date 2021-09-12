#include "mdhd.hpp"
#include <iostream>

MP4::mdhd::mdhd(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(atomBuild, filePath, filePos, pathParent)
{
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mdhd atom can not parse file: "+filePath);
    datablock::mdhdDataBlock mdhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &mdhdData, sizeof(mdhdData));
    timeScale = _byteswap_ulong(mdhdData.timeScale);
    duration = _byteswap_ulong(mdhdData.duration);
    fileStream.close();
}

void MP4::mdhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Header Atom)"<< std::endl;
    std::cout << dataIndent << "timeScale  : " << timeScale << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
}

void MP4::mdhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::mdhd::key = "mdhd";

