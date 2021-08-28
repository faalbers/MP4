#include "stsz.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stsz::stsz(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct stszDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    defaultSampleSize;             // if zero, all samples have different size.
        uint32_t    numberOfEntries;        // number of sample descriptions that follow
    } stszDataBlock;

    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsz atom can not parse file: "+filePath);
    stszDataBlock stszData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stszData, sizeof(stszData));
    defaultSampleSize = _byteswap_ulong(stszData.defaultSampleSize);
    if ( defaultSampleSize == 0 ) {
        stszData.numberOfEntries = _byteswap_ulong(stszData.numberOfEntries);
        auto index = stszData.numberOfEntries;
        uint32_t defaultSampleSizeTemp;
        do {
            fileStream.read((char *) &defaultSampleSizeTemp, sizeof(defaultSampleSizeTemp));
            stszTable.push_back(_byteswap_ulong(defaultSampleSizeTemp));
            index--;
        } while ( index > 0);
    }
    fileStream.close();
}

void MP4::stsz::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Size Atom)" << std::endl;
    std::cout << dataIndent << "defaultSamplSize: " << defaultSampleSize << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (sample Size)\n";
    if ( fullLists || (!fullLists && stszTable.size() <= 6) ) {
        for ( auto entry : stszTable ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stszTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = stszTable.size()-3 ; index < stszTable.size(); index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stszTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stsz::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stsz::key = "stsz";

