#include "stts.hpp"
#include <iostream>

MP4::stts::stts(atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stts atom can not parse file: "+filePath_);
    tableBlock sttsData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &sttsData, sizeof(sttsData));
    sttsData.numberOfEntries = XXH_swap32(sttsData.numberOfEntries);
    auto index = sttsData.numberOfEntries;
    uint32_t ID = 1;
    do {
        std::vector<uint32_t> sttsEntry;
        uint32_t val;
        fileStream.read((char *) &val, sizeof(val));
        sttsEntry.push_back(XXH_swap32(val));
        fileStream.read((char *) &val, sizeof(val));
        sttsEntry.push_back(XXH_swap32(val));
        sttsTable[ID] = sttsEntry;
        index--;
        ID++;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stts::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Time-To-Sample Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (sampleCount , sampleDuration)\n";
    for ( auto entry : sttsTable )
        std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second[0] << ", " << entry.second[1] << " )" << std::endl;
}

void MP4::stts::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stts::key = "stts";

