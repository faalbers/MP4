#include "stts.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stts::stts(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stts atom can not parse file: "+filePath);
    datablock::atomTableBlock sttsData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &sttsData, sizeof(sttsData));
    sttsData.numberOfEntries = _byteswap_ulong(sttsData.numberOfEntries);
    auto index = sttsData.numberOfEntries;
    do {
        std::vector<uint32_t> sttsEntry;
        uint32_t val;
        fileStream.read((char *) &val, sizeof(val));
        sttsEntry.push_back(_byteswap_ulong(val));
        fileStream.read((char *) &val, sizeof(val));
        sttsEntry.push_back(_byteswap_ulong(val));
        sttsTable.push_back(sttsEntry);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stts::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Time-To-Sample Atom)" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (sampleCount , sampleDuration)\n";
    for ( auto entry : sttsTable ) {
        std::cout << dataIndent << "[" << index << "] ( " << entry[0] << ", " << entry[1] << " )" << std::endl;
        index++;
    }
}

void MP4::stts::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stts::key = "stts";

