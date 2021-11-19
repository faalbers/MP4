#include "stco.hpp"
#include <iostream>

MP4::stco::stco(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock stcoData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &stcoData, sizeof(stcoData));
    stcoData.numberOfEntries = XXH_swap32(stcoData.numberOfEntries);
    auto index = stcoData.numberOfEntries;
    uint32_t chunkOffset;
    uint32_t ID = 1;
    do {
        fileStream->read((char*) &chunkOffset, sizeof(chunkOffset));
        chunkOffset = XXH_swap32(chunkOffset);
        stcoTable[ID] = (uint64_t) chunkOffset;
        index--;
        ID++;
    } while ( index > 0);
}

void MP4::stco::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Chunk Offset Atom (32-bit)) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] ( chunk offset )\n";
    if ( fullLists || (!fullLists && stcoTable.size() <= 6) ) {
        for ( auto entry : stcoTable )
            std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second << " )" << std::endl;
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stcoTable.at(index) << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) stcoTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stcoTable.at(index) << " )" << std::endl;
        }
    }
}

std::string MP4::stco::getKey() const
{
    return key;
}

const std::string MP4::stco::key = "stco";
