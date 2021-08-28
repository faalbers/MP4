#include "mdhd.hpp"
#include <iostream>

MP4::mdhd::mdhd(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct mdhdDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    creationTime;
        uint32_t    modificationTime;
        uint32_t    timeScale;          // time units per second
        uint32_t    duration;           // amount of timeScale units
    } mdhdDataBlock;

    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mdhd atom can not parse file: "+filePath);
    mdhdDataBlock mdhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &mdhdData, sizeof(mdhdData));
    timeScale = _byteswap_ulong(mdhdData.timeScale);
    duration = _byteswap_ulong(mdhdData.duration);
    fileStream.close();
}

void MP4::mdhd::printData()
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Header Atom)"<< std::endl;
    std::cout << dataIndent << "timeScale  : " << timeScale << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
}

void MP4::mdhd::printHierarchyData()
{
    printData();
    for ( auto child : children_ ) child->printHierarchyData();
}

std::string MP4::mdhd::key = "mdhd";

