#include "co64.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::co64::co64(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("co64 atom can not parse file: "+filePath_);
    datablock::atomTableBlock co64Data;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &co64Data, sizeof(co64Data));
    co64Data.numberOfEntries = XXH_swap32(co64Data.numberOfEntries);
    auto index = co64Data.numberOfEntries;
    uint64_t chunkOffset;
    do {
        fileStream.read((char *) &chunkOffset, sizeof(chunkOffset));
        chunkOffset = XXH_swap64(chunkOffset);
        co64Table.push_back(chunkOffset);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::co64::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Chunk Offset Atom (64 bit)) ["<< headerSize_ << "]" << std::endl;
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

void MP4::co64::createData(splunkType &splunk)
{
    // no checking of trackID since that is done in the trak level

    auto trackID = trakAtom_->getID();

    datablock::atomTableBlock stcoData;
    stcoData.version = 0;
    stcoData.flag[0] = 0;
    stcoData.flag[1] = 0;
    stcoData.flag[2] = 0;

    splunk.fileWrite->write((char *) &stcoData, sizeof(stcoData));
    auto entriesSizePos = splunk.fileWrite->tellp() - (int64_t) 4;
    uint32_t sampleCount = 0;
    for ( auto sample : splunk.samples ) {
        if ( sample.trackID == trackID ) {
            auto offset = XXH_swap64((uint64_t) sample.filePos);
            splunk.fileWrite->write((char *) &offset, sizeof(offset));
            sampleCount++;
        }
    }
    auto lastPos = splunk.fileWrite->tellp();
    splunk.fileWrite->seekp(entriesSizePos, splunk.fileWrite->beg);
    sampleCount = XXH_swap32(sampleCount);
    splunk.fileWrite->write((char *) &sampleCount, sizeof(sampleCount));
    splunk.fileWrite->seekp(lastPos, splunk.fileWrite->beg);
}

std::string MP4::co64::key = "co64";

