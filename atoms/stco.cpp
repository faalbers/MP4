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
    std::cout << path_ << " (Chunk Offset Atom (32-bit)) ["<< headerSize_ << "]" << std::endl;
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

void MP4::stco::createHeader(splunkType &splunk)
{
    createHeaderNew_(splunk, "co64", false);
}

void MP4::stco::createData(splunkType &splunk)
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
            //auto offset = _byteswap_ulong((uint32_t) sample.filePos);
            auto offset = _byteswap_uint64((uint64_t) sample.filePos);
            splunk.fileWrite->write((char *) &offset, sizeof(offset));
            sampleCount++;
        }
    }
    auto lastPos = splunk.fileWrite->tellp();
    splunk.fileWrite->seekp(entriesSizePos, splunk.fileWrite->beg);
    sampleCount = _byteswap_ulong(sampleCount);
    splunk.fileWrite->write((char *) &sampleCount, sizeof(sampleCount));
    splunk.fileWrite->seekp(lastPos, splunk.fileWrite->beg);
}

std::string MP4::stco::key = "stco";
