#include "stco.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stco::stco(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stco atom can not parse file: "+filePath_);
    datablock::atomTableBlock stcoData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stcoData, sizeof(stcoData));
    stcoData.numberOfEntries = _byteswap_ulong(stcoData.numberOfEntries);
    auto index = stcoData.numberOfEntries;
    uint32_t chunkOffset;
    do {
        fileStream.read((char *) &chunkOffset, sizeof(chunkOffset));
        chunkOffset = _byteswap_ulong(chunkOffset);
        stcoTable.push_back((uint64_t)chunkOffset);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stco::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Chunk Offset Atom (32-bit))" << std::endl;
    size_t index = 1;
    std::cout << dataIndent << "[#] ( chunk offset )\n";
    if ( fullLists || (!fullLists && stcoTable.size() <= 6) ) {
        for ( auto entry : stcoTable ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stcoTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = stcoTable.size()-3 ; index < stcoTable.size(); index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stcoTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stco::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::stco::writeData(std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    if ( writeInfo.chunkLists.size() == 0 ) {
        writeData_(fileWrite, writeInfo);
        return;
    }

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeData can not parse file: "+filePath_);
    
    // Handle atomTableBlock first
    datablock::atomTableBlock stcoData;
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read((char *) &stcoData, sizeof(stcoData));

    // recreate chunlist for this track
    std::vector<std::shared_ptr<chunkType>> chunkList;
    for ( auto chunk : writeInfo.chunkLists[0] )
        if ( chunk->trackID == trakAtom_->getID() )
            chunkList.push_back(chunk);

    // write data table block
    stcoData.numberOfEntries = _byteswap_ulong( (uint32_t) chunkList.size());
    fileRead.close();
    fileWrite.write((char *) &stcoData, sizeof(stcoData));

    // now add all chunk offset values in uint32_t
    for ( auto chunk : chunkList ) {
        auto chunkOffset = (uint32_t ) chunk->dataOffset;
        chunkOffset = _byteswap_ulong(chunkOffset);
        fileWrite.write((char *) &chunkOffset, sizeof(chunkOffset));
    }
}

std::string MP4::stco::key = "stco";
