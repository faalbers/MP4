#include "stss.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stss::stss(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct stssDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    numberOfEntries;        // number of sample descriptions that follow
    } stssDataBlock;

    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stss atom can not parse file: "+filePath);
    stssDataBlock stssData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stssData, sizeof(stssData));
    stssData.numberOfEntries = _byteswap_ulong(stssData.numberOfEntries);
    auto index = stssData.numberOfEntries;
    uint32_t numberTemp;
    do {
        fileStream.read((char *) &numberTemp, sizeof(numberTemp));
        stssTable.push_back(_byteswap_ulong(numberTemp));
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stss::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sync Sample Atom)" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (sample number)\n";
    if ( fullLists || (!fullLists && stssTable.size() <= 6) ) {
        for ( auto entry : stssTable ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stssTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = stssTable.size()-3 ; index < stssTable.size(); index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stssTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stss::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stss::key = "stss";

