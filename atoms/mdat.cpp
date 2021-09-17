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

void MP4::mdat::writeData(std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    std::set<uint32_t> includeTrackIDs;
    std::vector<std::shared_ptr<chunkType>> chunkList;

    // write mdat data
    for ( auto entry : writeInfo.includeTrackIDs ) includeTrackIDs.insert(entry.first);
    extract_(fileWrite, chunkList, includeTrackIDs);
    writeInfo.chunkLists.push_back(chunkList);
}

void MP4::mdat::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    std::set<uint32_t> includeTrackIDs;
    std::vector<std::shared_ptr<chunkType>> chunkList;

    // write main mdat data
    for ( auto entry : writeInfo.includeTrackIDs ) includeTrackIDs.insert(entry.first);
    extract_(fileWrite, chunkList, includeTrackIDs);
    writeInfo.chunkLists.push_back(chunkList);

    includeTrackIDs.clear();
    chunkList.clear();
    for ( auto entry : writeInfo.includeTrackIDs ) includeTrackIDs.insert(entry.second);
    ((mdat *) appendAtom)->extract_(fileWrite, chunkList, includeTrackIDs);
    writeInfo.chunkLists.push_back(chunkList);
}

std::tuple<int64_t, bool> MP4::mdat::appendHeader(std::ofstream &fileWrite)
{
    // making mdat size 64 bit
    char    *buffer;
    size_t  bufferSize;
    
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);
    fileRead.seekg(filePos_, fileRead.beg);

    // creating a 64 bit header for mdat
    datablock::atomHeaderBlock atomHeader;
    fileRead.read((char *) &atomHeader, sizeof(atomHeader));
    fileRead.close();
    
    atomHeader.size32 = _byteswap_ulong((uint32_t) 1);
    atomHeader.size64 = _byteswap_uint64((uint64_t) 16);
    auto writeSizePos = fileWrite.tellp();
    fileWrite.write((char *) &atomHeader, sizeof(atomHeader));

    return make_tuple(writeSizePos, true);
}

void MP4::mdat::extract_(std::ofstream &fileWrite,
    std::vector<std::shared_ptr<chunkType>> &chunkList,
    std::set<uint32_t> &includeTrackIDs)
{
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);

    std::map<uint64_t, chunkType> chunkMap;
    std::map<uint32_t, std::vector<sampleType>> trackSsamples;
    for ( auto track : moovAtom_->getTypeAtoms<trak>() ) {
        auto it = includeTrackIDs.find(track->getID());
        if( it == includeTrackIDs.end() ) continue;
        for ( auto chunk : track->getChunks() )
            chunkMap[chunk.dataOffset] = chunk;
        trackSsamples[track->getID()] = track->getSamples().samples;
    }
    
    uint32_t dataSize;
    size_t bufferSize;
    char *buffer;
    for ( auto chunk : chunkMap ) {
        dataSize = 0;
        for ( uint32_t index = 0; index < chunk.second.samples; index++ ) {
            auto sampleIndex = chunk.second.firstSampleID-1+index;
            dataSize += trackSsamples[chunk.second.trackID][sampleIndex].size;
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
        if ( chunkList.size() == 0)
            sharedCHunk->ID = 1;
        else
            sharedCHunk->ID = chunkList.back()->ID + 1;
        chunkList.push_back(sharedCHunk);
    }
}

std::string MP4::mdat::key = "mdat";

