#include "mdat.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <map>

MP4::mdat::mdat(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    sampleDataPos = fileDataPos_;
    sampleDataSize = dataSize_;
}

void MP4::mdat::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Data Atom)" << std::endl;
    std::cout << dataIndent << "filePosition of sample data : " << sampleDataPos << std::endl;
    std::cout << dataIndent << "Size of sample data in bytes: " << sampleDataSize << std::endl;
}

void MP4::mdat::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::tuple<int64_t, bool> MP4::mdat::writeHeader(std::ofstream &fileWrite)
{
    char    *buffer;
    size_t  bufferSize;
    
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);
    fileRead.seekg(filePos_, fileRead.beg);

    // creating a 64 bit header for mdat
    datablock::atomHeaderBlock atomHeader;
    fileRead.read((char *) &atomHeader, sizeof(atomHeader));
    atomHeader.size32 = _byteswap_ulong((uint32_t) 1);
    atomHeader.size64 = _byteswap_uint64((uint64_t) 16);
    auto writeSizePos = fileWrite.tellp();
    fileWrite.write((char *) &atomHeader, sizeof(atomHeader));

    return make_tuple(writeSizePos, true);
}

void MP4::mdat::writeData(std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    //extract_(fileWrite, writeInfo); // this is most efficient

}

void MP4::mdat::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    //extract_(fileWrite, writeInfo);
    //((mdat *) appendAtom)->extract_(fileWrite, writeInfo);
}

void MP4::mdat::extract_(std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);

    std::map<uint64_t, chunkType> chunkMap;
    std::map<uint32_t, std::vector<sampleType>> trackSsamples;
    for ( auto track : writeInfo.moovAtom->getTypeAtoms<trak>() ) {
        auto it = writeInfo.includeTrackIDs.find(track->getID());
        if( it == writeInfo.includeTrackIDs.end() ) continue;
        for ( auto chunk : track->getChunks() )
            chunkMap[chunk.dataOffset] = chunk;
        trackSsamples[track->getID()] = track->getSamples();
    }
    
    uint32_t dataSize;
    size_t bufferSize;
    char *buffer;
    for ( auto chunk : chunkMap ) {
        dataSize = 0;
        for ( uint32_t index = 0; index < chunk.second.samples; index++ ) {
            auto sampleIndex = chunk.second.firstSampleID-1+index;
            dataSize += trackSsamples[chunk.second.trackID][sampleIndex].dataSize;
        }
        bufferSize = (size_t) dataSize;
        buffer = new char[bufferSize];
        fileRead.seekg((int64_t) chunk.first, fileRead.beg);

        // set new dataOffset
        chunk.second.dataOffset = (uint64_t) fileWrite.tellp();

        fileRead.read(buffer, bufferSize);
        fileWrite.write(buffer, bufferSize);
        delete[] buffer;

        // add chunk to list for stco or co64
        auto sharedCHunk = std::make_shared<chunkType>();
        *sharedCHunk = chunk.second;
        // update chunk ID if write is appended
        if ( writeInfo.chunkList.size() == 0)
            sharedCHunk->ID = 1;
        else
            sharedCHunk->ID = writeInfo.chunkList.back()->ID + 1;
        writeInfo.chunkList.push_back(sharedCHunk);
    }
}

std::string MP4::mdat::key = "mdat";

