#include "moov.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <iomanip>
#include <set>
#include <map>

MP4::moov::moov(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
}

void MP4::moov::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::moov::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::moov::writeChildrenToFile_(std::ofstream &fileWrite, char *data)
{
    if ( data == nullptr)
        for ( auto child : children_ ) {
            child->writeToFile(fileWrite, data);
            return;
        }
    auto writeInfo = (writeInfoType *) data;

    for ( auto child : children_ ) {
        if ( child->key == "trak" ) {
            // only write track if included in info
            auto track = (trak *) child.get();
            auto trackID = track->getID();
            std::set<uint32_t>::iterator it = writeInfo->includeTrackIDs.find(trackID);
            if( it != writeInfo->includeTrackIDs.end() ) {
                writeInfo->currentTrackID = trackID;
                child->writeToFile(fileWrite, data);
            }
            continue;
        }
        child->writeToFile(fileWrite, data);
    }
}

void MP4::moov::writeToFile(std::ofstream &fileWrite, char *data)
{
    if ( filePath_ == "") {
        writeToFile_(fileWrite, data);
        return;
    }
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);
    fileRead.seekg(filePos_, fileRead.beg);

    // write mdat header
    int64_t writeSizePos = fileWrite.tellp();
    uint32_t mdatSize = 8;
    mdatSize = _byteswap_ulong(mdatSize);
    char atomKey[] = {'m', 'd', 'a', 't'};
    fileWrite.write((char *) &mdatSize, 4);
    fileWrite.write(atomKey, 4);

    writeInfoType writeInfo;
    std::vector<std::vector<chunkType>> trackChunks;
    std::vector<std::vector<sampleType>> samples;
    uint32_t largestTimeScale = 0;
    for ( auto track : getTypeAtoms<trak>() ) {
        auto chunks = track->getChunks();
        std::reverse(chunks.begin(),chunks.end());
        trackChunks.push_back(chunks);
        samples.push_back(track->getSamples());
        for ( auto mdhd : track->getTypeAtoms<mdhd>() )
            if ( mdhd->timeScale > largestTimeScale ) largestTimeScale = mdhd->timeScale;
    }

    bool chunksDepleted = false;
    do {
        std::map<uint64_t, chunkType> chunkMap;
        int trackIndex = 0;
        for ( auto chunks : trackChunks ) {
            if ( chunks.size() > 0 ) {
                auto trackSamples = samples[trackIndex];
                auto startSampleID = chunks.back().firstSampleID;
                auto endSampleID = startSampleID + chunks.back().samples - 1;
                auto startTime = trackSamples[startSampleID-1].time;
                auto endTime = trackSamples[endSampleID-1].time + trackSamples[endSampleID-1].duration;
                if ( startTime == endTime ) {
                    trackIndex++;
                    continue;
                }
                auto timeScaleMult = (double) largestTimeScale / trackSamples[startSampleID-1].timeScale;
                auto timeScaled = (uint32_t) (timeScaleMult * startTime);
                chunkMap[timeScaled] = chunks.back();
                writeInfo.includeTrackIDs.insert(chunks.back().trackID);
            }
            trackIndex++;
        }
        if ( chunkMap.size() > 0 ) {
            auto time = (*(chunkMap. begin())).first;
            auto chunk = (*(chunkMap. begin())).second;
            auto trackSamples = samples[chunk.trackID-1];
            auto startSampleID = chunk.firstSampleID;
            auto endSampleID = startSampleID + chunk.samples - 1;
            uint32_t chunkSize = 0;
            for ( uint32_t sampleID = startSampleID; sampleID <= endSampleID; sampleID++ )
                chunkSize += trackSamples[sampleID-1].dataSize;
            //std::cout << chunk.dataOffset << " " << chunk.dataOffset+(uint64_t) chunkSize << std::endl;
            auto bufferSize = (size_t) chunkSize;
            char *buffer = new char[bufferSize];
            fileRead.seekg((int64_t) chunk.dataOffset, fileRead.beg);

            // set new dataOffset
            chunk.dataOffset = (uint64_t) fileWrite.tellp();

            fileRead.read(buffer, bufferSize);
            fileWrite.write(buffer, bufferSize);
            delete[] buffer;

            // add chunk to list for stco or co64
            writeInfo.chunkList.push_back(chunk);

            if ( trackChunks[chunk.trackID-1].size() != 0 )
                trackChunks[chunk.trackID-1].pop_back();
        }  else chunksDepleted = true;
    } while ( !chunksDepleted );
    fileRead.close();

    // set mdat size
    auto writeNextPos = fileWrite.tellp();
    auto writeSize = (uint32_t) (writeNextPos - writeSizePos);
    writeSize = _byteswap_ulong(writeSize);
    fileWrite.seekp(writeSizePos, fileWrite.beg);
    fileWrite.write((char *) &writeSize, sizeof(writeSize));
    fileWrite.seekp(writeNextPos, fileWrite.beg);

    std::cout << "Chunks written: " << writeInfo.chunkList.size() << std::endl;

    writeToFile_(fileWrite, (char *) &writeInfo);
}

std::string MP4::moov::key = "moov";

