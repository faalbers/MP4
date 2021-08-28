#include "co64.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::co64::co64(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct co64DataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    numberOfEntries;        // (32-bit integer) number of sample descriptions that follow
    } co64DataBlock;

    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("co64 atom can not parse file: "+filePath);
    co64DataBlock co64Data;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &co64Data, sizeof(co64Data));
    co64Data.numberOfEntries = _byteswap_ulong(co64Data.numberOfEntries);
    auto index = co64Data.numberOfEntries;
    uint64_t chunkOffset;
    do {
        fileStream.read((char *) &chunkOffset, sizeof(chunkOffset));
        chunkOffset = _byteswap_uint64(chunkOffset);
        co64Table.push_back(chunkOffset);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::co64::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Chunk Offset Atom (64 bit))" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] ( chunk offset )\n";
    if ( fullLists || (!fullLists && co64Table.size() <= 6) ) {
        for ( auto entry : co64Table ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << co64Table[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = co64Table.size()-3 ; index < co64Table.size(); index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << co64Table[index] << " )" << std::endl;
        }
    }
}

void MP4::co64::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::co64::key = "co64";
