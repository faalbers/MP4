#include "mdat.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <map>

MP4::mdat::mdat(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
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

void MP4::mdat::writeAtomDataToFile(std::ofstream &fileWrite, char *data)
{
    if ( data == nullptr || filePath_ == "" ) {
        writeAtomDataToFile_(fileWrite, data);
        return;
    }

    //timeReshuffle_(fileWrite, data);
    extract_(fileWrite, data); // this is most efficient
    //writeAtomDataToFile_(fileWrite, data);
}

void MP4::mdat::appendData(atom *appendAtom, std::ofstream &fileWrite, char *data)
{
    appendData_(appendAtom, fileWrite, data);
}

void MP4::mdat::extract_(std::ofstream &fileWrite, char *data)
{
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);

    auto writeInfo = (internal::writeInfoType *) data;

    std::map<uint64_t, chunkType> chunkMap;
    std::map<uint32_t, std::vector<sampleType>> trackSsamples;
    for ( auto track : writeInfo->moovAtom->getTypeAtoms<trak>() ) {
        std::set<uint32_t>::iterator it = writeInfo->excludeTrackIDs.find(track->getID());
        if( it != writeInfo->excludeTrackIDs.end() ) continue;
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
        writeInfo->chunkList.push_back(sharedCHunk);
    }
}

void MP4::mdat::timeReshuffle_(std::ofstream &fileWrite, char *data)
{
    auto writeInfo = (internal::writeInfoType *) data;

    // creating our own time sorted mdat
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);

    std::map<uint32_t, std::vector<chunkType>> trackChunks;
    std::map<uint32_t, std::vector<sampleType>> samples;
    uint32_t largestTimeScale = 0;
    // a vector of each track's chunks and reverse them for pop action
    // get samples for each track for timing search
    // get timescales of all tracks and use the largest one
    for ( auto track : writeInfo->moovAtom->getTypeAtoms<trak>() ) {
        // exclude tracks from exclusion list
        std::set<uint32_t>::iterator it = writeInfo->excludeTrackIDs.find(track->getID());
        if( it != writeInfo->excludeTrackIDs.end() ) continue;

        auto chunks = track->getChunks();
        std::reverse(chunks.begin(),chunks.end());
        trackChunks[track->getID()] = chunks;
        samples[track->getID()] = track->getSamples();
        for ( auto mdhd : track->getTypeAtoms<mdhd>() )
            if ( mdhd->timeScale > largestTimeScale ) largestTimeScale = mdhd->timeScale;
    }

    bool chunksDepleted = false;
    do {
        std::map<uint64_t, chunkType> chunkMap;
        int trackIndex = 0;
        // map of all current top chunks of each track
        // find their timing by checking samples
        // create map based on timing the earliest ones will be first
        for ( auto chunks : trackChunks ) {
            if ( chunks.second.size() > 0 ) {
                auto trackSamples = samples[chunks.second.back().trackID];
                auto startSampleID = chunks.second.back().firstSampleID;
                auto endSampleID = startSampleID + chunks.second.back().samples - 1;
                auto startTime = trackSamples[startSampleID-1].time;
                auto endTime = trackSamples[endSampleID-1].time + trackSamples[endSampleID-1].duration;
                // for now we don't handle tracks with zero duration samples
                // not sure on the sorting scheme for those yet
                // they are used by GoPro to fix corrupted files
                if ( startTime == endTime ) {
                    writeInfo->excludeTrackIDs.insert(chunks.second.back().trackID);
                    trackIndex++;
                    continue;
                }
                auto timeScaleMult = (double) largestTimeScale / trackSamples[startSampleID-1].timeScale;
                auto timeScaled = (uint32_t) (timeScaleMult * startTime);
                chunkMap[timeScaled] = chunks.second.back();
            }
            trackIndex++;
        }

        // handle the earliest chunk and add it to the sorte chunk list
        // pop the chunk that was handled
        // keep on doing this till all chunk lists of all track are depleted
        if ( chunkMap.size() > 0 ) {
            auto time = (*(chunkMap. begin())).first;
            auto chunk = (*(chunkMap. begin())).second;
            auto trackSamples = samples[chunk.trackID];
            auto startSampleID = chunk.firstSampleID;
            auto endSampleID = startSampleID + chunk.samples - 1;
            uint32_t chunkSize = 0;
            for ( uint32_t sampleID = startSampleID; sampleID <= endSampleID; sampleID++ )
                chunkSize += trackSamples[sampleID-1].dataSize;
            auto bufferSize = (size_t) chunkSize;
            char *buffer = new char[bufferSize];
            fileRead.seekg((int64_t) chunk.dataOffset, fileRead.beg);

            // set new dataOffset
            chunk.dataOffset = (uint64_t) fileWrite.tellp();

            fileRead.read(buffer, bufferSize);
            fileWrite.write(buffer, bufferSize);
            delete[] buffer;

            // add chunk to list for stco or co64
            auto sharedCHunk = std::make_shared<chunkType>();
            *sharedCHunk = chunk;
            writeInfo->chunkList.push_back(sharedCHunk);

            if ( trackChunks[chunk.trackID].size() != 0 )
                trackChunks[chunk.trackID].pop_back();
        }  else chunksDepleted = true;
    } while ( !chunksDepleted );
    fileRead.close();
}

std::string MP4::mdat::key = "mdat";

