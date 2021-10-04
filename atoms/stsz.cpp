#include "stsz.hpp"
#include <iostream>

MP4::stsz::stsz(atomBuildType &atomBuild)
    : atom(atomBuild)
{
    typedef struct tableBlock
    {
        versionBlock    version;
        uint32_t        defaultSampleSize;             // if zero, all samples have different size.
        uint32_t        numberOfEntries;        // number of sample descriptions that follow
    } tableBlock;

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsz atom can not parse file: "+filePath_);
    tableBlock stszData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stszData, sizeof(stszData));
    defaultSampleSize = XXH_swap32(stszData.defaultSampleSize);
    if ( defaultSampleSize == 0 ) {
        stszData.numberOfEntries = XXH_swap32(stszData.numberOfEntries);
        auto index = stszData.numberOfEntries;
        uint32_t defaultSampleSizeTemp;
        uint32_t ID = 1;
        do {
            fileStream.read((char *) &defaultSampleSizeTemp, sizeof(defaultSampleSizeTemp));
            stszTable[ID] = XXH_swap32(defaultSampleSizeTemp);
            index--;
            ID++;
        } while ( index > 0);
    }
    fileStream.close();
}

void MP4::stsz::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Size Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "defaultSamplSize: " << defaultSampleSize << std::endl;
    if ( stszTable.size() != 0 ) std::cout << dataIndent << "[#] (sample Size)\n";
    if ( fullLists || (!fullLists && stszTable.size() <= 6) ) {
        for ( auto entry : stszTable )
            std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second << " )" << std::endl;
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stszTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) stszTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stszTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stsz::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stsz::key = "stsz";

