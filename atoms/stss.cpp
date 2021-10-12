#include "stss.hpp"
#include <iostream>

MP4::stss::stss(atomParse parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    tableBlock stssData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &stssData, sizeof(stssData));
    stssData.numberOfEntries = XXH_swap32(stssData.numberOfEntries);
    auto index = stssData.numberOfEntries;
    uint32_t sampleID;
    uint32_t ID = 1;
    do {
        fileStream->read((char *) &sampleID, sizeof(sampleID));
        stssTable[ID] = XXH_swap32(sampleID);
        index--;
        ID++;
    } while ( index > 0);
}

void MP4::stss::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sync Sample Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (sample ID)\n";
    if ( fullLists || (!fullLists && stssTable.size() <= 6) ) {
        for ( auto entry : stssTable )
            std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second << " )" << std::endl;
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stssTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) stssTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stssTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stss::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stss::key = "stss";

