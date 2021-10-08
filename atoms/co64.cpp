#include "co64.hpp"
#include <iostream>

MP4::co64::co64(atomBuild &build)
    : atom(build)
{
    // handle data 
    auto fileStream = build.getFileStream();

    tableBlock co64Data;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &co64Data, sizeof(co64Data));
    co64Data.numberOfEntries = XXH_swap32(co64Data.numberOfEntries);
    auto index = co64Data.numberOfEntries;
    uint64_t chunkOffset;
    uint32_t ID = 1;
    do {
        fileStream->read((char *) &chunkOffset, sizeof(chunkOffset));
        chunkOffset = XXH_swap64(chunkOffset);
        co64Table[ID] = chunkOffset;
        index--;
        ID++;
    } while ( index > 0);
}

void MP4::co64::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Chunk Offset Atom (64 bit)) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] ( chunk offset )\n";
    if ( fullLists || (!fullLists && co64Table.size() <= 6) ) {
        for ( auto entry : co64Table )
            std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second << " )" << std::endl;
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << co64Table[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) co64Table.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << co64Table[index] << " )" << std::endl;
        }
    }
}

void MP4::co64::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::co64::key = "co64";

