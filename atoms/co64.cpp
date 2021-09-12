#include "co64.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::co64::co64(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos)
    : atom(atomBuild, filePath, filePos)
{
    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("co64 atom can not parse file: "+filePath);
    datablock::atomTableBlock co64Data;
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
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Chunk Offset Atom (64 bit))" << std::endl;
    size_t index = 1;
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
/*
void MP4::co64::writeAtomDataToFile(std::ofstream &fileWrite, char *data)
{
    if ( data == nullptr || filePath_ == "" ) {
        writeAtomDataToFile_(fileWrite, data);
        return;
    }
    auto writeInfo = (internal::writeInfoType *) data;

    if ( writeInfo->chunkList.size() == 0 ) {
        writeAtomDataToFile_(fileWrite, data);
        return;
    }

    // Handle atomTableBlock first
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeChildrenToFile_ can not parse file: "+filePath_);
    
    datablock::atomTableBlock stcoData;
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read((char *) &stcoData, sizeof(stcoData));
    
    uint32_t numberOfEntries = 0;
    for ( auto chunk : writeInfo->chunkList )
        if ( chunk->trackID == writeInfo->currentTrackID )
            numberOfEntries++;

    std::cout << numberOfEntries << std::endl;
    stcoData.numberOfEntries = _byteswap_ulong(numberOfEntries);
    fileRead.close();
    fileWrite.write((char *) &stcoData, sizeof(stcoData));

    // now add all chunk offset values in uint64_t
    for ( auto chunk : writeInfo->chunkList ) {
        if ( chunk->trackID == writeInfo->currentTrackID ) {
            auto chunkOffset = chunk->dataOffset;
            chunkOffset = _byteswap_uint64(chunkOffset);
            fileWrite.write((char *) &chunkOffset, sizeof(chunkOffset));
        }
    }
}
*/
std::string MP4::co64::key = "co64";

