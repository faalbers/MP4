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
    std::cout << path_ << " (Media Data Atom) ["<< headerSize_ << "]" << std::endl;
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

void MP4::mdat::createHeader(splunkType &splunk)
{
    createHeaderNew_(splunk, key, true);
}

void MP4::mdat::createData(splunkType &splunk)
{
    //createData_(splunk);
    //return;

    if (  splunk.samples.size() == 0 ) {
        createData_(splunk);
        return;
    }
    
    // find largest sample size
    size_t bufferSize = 0;
    for ( auto sample : splunk.samples )
        if ( sample.size > bufferSize ) bufferSize = sample.size;

    std::string readPath = splunk.samples[0].filePath;
    std::ifstream fileRead(readPath, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat::createData can not parse file: "+readPath);

    auto buffer = new char[bufferSize];
    for ( size_t index = 0; index < splunk.samples.size(); index++ ) {
        if ( splunk.samples[index].filePath != readPath ) {
            readPath = splunk.samples[index].filePath;
            fileRead.close();
            fileRead = std::ifstream(readPath, std::ios::binary);
            if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat::createData can not parse file: "+readPath);
        }

        // read sample
        fileRead.seekg(splunk.samples[index].filePos, fileRead.beg);
        fileRead.read(buffer, splunk.samples[index].size);

        // update sample filePos and path
        splunk.samples[index].filePos = splunk.fileWrite->tellp();
        splunk.samples[index].filePath = splunk.fileWritePath;

        splunk.fileWrite->write(buffer, splunk.samples[index].size);
    }
    delete[] buffer;

    fileRead.close();
/*

    // find largest sample size
    size_t bufferSize = 0;
    for ( auto sample : splunk.samples )
        if ( sample.size > bufferSize ) bufferSize = sample.size;

    std::string readPath = splunk.samples[0].filePath;
    std::ifstream fileRead(readPath, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat::createData can not parse file: "+readPath);

    //now copy sample buffers
    auto buffer = new char[bufferSize];
    for ( size_t index = 0; index < splunk.samples.size(); index++ ) {

        if ( splunk.samples[index].filePath != readPath ) {
            readPath = splunk.samples[index].filePath;
            fileRead.close();
            fileRead = std::ifstream(readPath, std::ios::binary);
            if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat::createData can not parse file: "+readPath);
        }
        fileRead.seekg(splunk.samples[index].filePos, fileRead.beg);
        fileRead.read(buffer, splunk.samples[index].size);

        auto matchingTracks = splunk.trackMatchB[splunk.samples[index].filePath][splunk.samples[index].trackID];
        if ( matchingTracks.size() == 0 ) std::cout << splunk.samples[index].filePath << " " << splunk.samples[index].trackID << std::endl;
        splunk.samples[index].filePos = (uint64_t) splunk.fileWrite->tellp();
        splunk.samples[index].filePath = splunk.fileWritePath;

        splunk.fileWrite->write(buffer, splunk.samples[index].size);
    }
    delete[] buffer;

    fileRead.close();
*/
}

std::string MP4::mdat::key = "mdat";

