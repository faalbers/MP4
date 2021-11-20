#include "co64.hpp"
#include <iostream>

MP4::co64::co64(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock co64Data;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &co64Data, sizeof(co64Data));
    co64Data.numberOfEntries = XXH_swap32(co64Data.numberOfEntries);
    auto index = co64Data.numberOfEntries;
    uint64_t chunkOffset;
    uint32_t ID = 1;
    do {
        fileStream->read((char*) &chunkOffset, sizeof(chunkOffset));
        chunkOffset = XXH_swap64(chunkOffset);
        co64Table[ID] = chunkOffset;
        index--;
        ID++;
    } while ( index > 0);
}

MP4::co64::co64(std::shared_ptr<atomBuild> build)
    : atom(build)
    , writeTrackID_(build->currentTrackID())
{
    // setting current trackID for write
    // no data to add. se write for dataOffsets
}

void MP4::co64::printData(bool fullLists) const
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
            std::cout << dataIndent << "[" << index << "] ( " << co64Table.at(index) << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) co64Table.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << co64Table.at(index) << " )" << std::endl;
        }
    }
}

std::string MP4::co64::getKey() const
{
    return key;
}

void MP4::co64::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock co64Data;

    // default settings
    co64Data.version.version = 0;
    co64Data.version.flag[0] = 0;
    co64Data.version.flag[1] = 0;
    co64Data.version.flag[2] = 0;
    
    // data settings
    //co64Data.numberOfEntries = XXH_swap32(writeFile->mdatWriteInfo);
    co64Data.numberOfEntries = XXH_swap32( (uint32_t) writeFile->mdatWriteInfo[writeTrackID_].size());

    fileWrite->write((char*) &co64Data, sizeof(co64Data));

    // write table
    uint64_t val;
    for ( auto dataOffset : writeFile->mdatWriteInfo[writeTrackID_] ) {
        val = XXH_swap64(dataOffset);
        fileWrite->write((char*) &val, sizeof(val));
    }
}

const std::string MP4::co64::key = "co64";

