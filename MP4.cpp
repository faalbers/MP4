#include "MP4.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <map>
#ifdef MP4_PARSE_TIME
#include <chrono>
#endif

MP4::MP4::MP4(std::string fileName)
{
    #ifdef MP4_PARSE_TIME
    auto start = std::chrono::high_resolution_clock::now();
    #endif
    
    filePath = std::filesystem::absolute(std::filesystem::path(fileName)).string();

    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("MP4 can not open file: " + filePath);

    // get file length
    fileStream.seekg(0, fileStream.end);
    fileSize = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);
    fileStream.close();

    int64_t nextFilePos = 0;
    do {
        auto child = atom::makeAtom_(filePath, nextFilePos);
        if ( child != nullptr ) {
            nextFilePos = child->fileNextPos_;
            children.push_back(child);
        }
    } while ( nextFilePos < fileSize );
    
    #ifdef MP4_PARSE_TIME
    auto end = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nMP4 Time : " << ms_int.count() << "ms\n";
    #endif
}

std::vector<MP4::trak *>  MP4::MP4::getTracks()
{
    for ( auto moov : getTypeAtoms<moov>()) return moov->getTypeAtoms<trak>();
    std::vector<trak *> foundTracks;
    return foundTracks;
}

MP4::trak *MP4::MP4::getTrackFromID(uint32_t ID)
{
    for ( auto track : getTracks() )
        if ( track->getID() == ID ) return track;
    return nullptr;
}

int MP4::MP4::nestLevel()
{
    int level, maxLevel = 0;
    for ( auto child : children ) {
        level = child->nestLevel_(0);
        if ( level > maxLevel ) maxLevel = level;
    }
    return maxLevel;
}

void MP4::MP4::printHierarchy()
{
    int pathWith = nestLevel() * 5;
    int valWith = 10;
    std::cout << std::string(pathWith, '-') << std::string(valWith*5 + 28, '-') << std::endl;
    std::cout << std::setw(pathWith) << std::left << "<path>";
    std::cout <<"  |- " << std::setw(valWith) << std::right << "<start>"
        << " -|- " << std::setw(valWith) << "<data>"
        << " -|- " << std::setw(valWith) << "<end>"
        << " -| ( " << std::setw(valWith) << "<size>"
        << " ) ( " << std::setw(valWith) << "<datasize>"
        << " )";
    std::cout << std::endl << std::string(pathWith, '-') << std::string(valWith*5 + 28, '-') << std::endl;

    for ( auto child : children ) child->printHierarchy(pathWith, valWith);
    
    std::cout << std::endl;
}

void MP4::MP4::printHierarchyData(bool fullLists)
{
    std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    for ( auto child : children ) child->printHierarchyData(fullLists);
}

void MP4::MP4::mdatRemap()
{
    std::vector<std::vector<chunkType>> trackChunks;
    std::vector<std::vector<sampleType>> samples;
    uint32_t largestTimeScale = 0;
    for ( auto track : getTracks() ) {
        auto chunks = track->getChunks();
        std::reverse(chunks.begin(),chunks.end());
        trackChunks.push_back(chunks);
        samples.push_back(track->getSamples());
        for ( auto mdhd : track->getTypeAtoms<mdhd>() )
            if ( mdhd->timeScale > largestTimeScale ) largestTimeScale = mdhd->timeScale;
    }


    bool chunksDepleted = false;
    std::cout << "reference timescale: " << largestTimeScale << std::endl;
    std::cout << "|    startPos|      endPos| startTime|   endTime|track|  chunk|\n";
    do {
        std::map<uint64_t, chunkType> chunkMap;
        int trackIndex = 0;
        for ( auto chunks : trackChunks ) {
            auto trackSamples = samples[trackIndex];
            if ( chunks.size() > 0 ) {
                auto startSampleID = chunks.back().firstSampleID;
                auto endSampleID = startSampleID + chunks.back().samples - 1;
                auto startTime = trackSamples[startSampleID-1].time;
                auto endTime = trackSamples[endSampleID-1].time + trackSamples[endSampleID-1].duration;
                
                auto timeScaleMult = (double) largestTimeScale / trackSamples[startSampleID-1].timeScale;
                auto timeScaled = (uint32_t) (timeScaleMult * startTime);
                
                chunkMap[timeScaled] = chunks.back();
            }
            trackIndex++;
        }
        std::vector<chunkType> exportChunks;
        if ( chunkMap.size() > 0 ) {
            auto minTime = (*(chunkMap. begin())).first;
            for ( std::pair chunk : chunkMap ) {
                if ( chunk.first == minTime ) {
                    std::cout   << "|" << std::setw(12) << chunk.second.dataOffset;
                    std::cout   << "|" << std::setw(12) << 0;
                    std::cout   << "|" << std::setw(10) << chunk.first;
                    std::cout   << "|" << std::setw(10) << 0;
                    std::cout   << "|" << std::setw(4) << chunk.second.trackID << "t"
                        << "|" << std::setw(6) << chunk.second.ID  << "c|" << std::endl;
                    if ( trackChunks[chunk.second.trackID-1].size() != 0 ) {
                        trackChunks[chunk.second.trackID-1].pop_back();
                    }
                }
            }
        } else chunksDepleted = true;
    } while ( !chunksDepleted );
}

void MP4::MP4::mdatMap(){
    std::map<uint64_t, chunkType> chunkMap;
    uint32_t largestTimeScale = 0;
    for ( auto track : getTracks() ) {
        for ( auto chunk : track->getChunks() )
            chunkMap[chunk.dataOffset] = chunk;
        for ( auto mdhd : track->getTypeAtoms<mdhd>() )
            if ( mdhd->timeScale > largestTimeScale ) largestTimeScale = mdhd->timeScale;
    }

    std::cout << "reference timescale: " << largestTimeScale << std::endl;
    std::cout << "|    startPos|      endPos| startTime|   endTime|track|  chunk|\n";
    for ( std::pair chunk : chunkMap ) {
        auto track = getTrackFromID(chunk.second.trackID);
        if ( track == nullptr ) continue;
        auto trackSamples = track->getSamples();

        auto startSampleID = chunk.second.firstSampleID;
        auto endSampleID = startSampleID + chunk.second.samples - 1;

        auto startPos = chunk.second.dataOffset;
        auto endPos = startPos;
        for ( uint32_t sampleID = startSampleID; sampleID <= endSampleID; sampleID++ )
            endPos += (uint64_t) trackSamples[sampleID-1].dataSize;
        
        std::cout   << "|" << std::setw(12) << startPos;
        std::cout   << "|" << std::setw(12) << endPos;
        
        auto startTime = trackSamples[startSampleID-1].time;
        auto endTime = trackSamples[endSampleID-1].time + trackSamples[endSampleID-1].duration;
        
        
        auto startTimeScaleMult = (double) largestTimeScale / trackSamples[startSampleID-1].timeScale;
        auto startTimeScaled = (uint32_t) (startTimeScaleMult * startTime);
        
        std::cout   << "|" << std::setw(10) << startTimeScaled;

        auto endTimeScaleMult = (double) largestTimeScale / trackSamples[endSampleID-1].timeScale;
        auto endTimeScaled = (uint32_t) (endTimeScaleMult * endTime);
        
        std::cout   << "|" << std::setw(10) << endTimeScaled;

        std::cout   << "|" << std::setw(4) << chunk.second.trackID << "t"
            << "|" << std::setw(6) << chunk.second.ID  << "c|" << std::endl;

    }
}

void MP4::MP4::writeFile(std::string filePath_, writeSettingsType &writeSettings)
{
    std::ofstream fileWrite(filePath_, std::ios::binary);
    if ( fileWrite.fail() ) throw std::runtime_error("Can not create MP4 file: "+filePath_);

    // data to pass through write process
    internal::writeInfoType writeInfo;
    // passing moov as data to atoms t0 reconstruct stuff if needed
    for ( auto moov : getTypeAtoms<moov>() )
        writeInfo.moovAtom = moov;
    writeInfo.excludeTrackIDs = writeSettings.excludeTrackIDs;
    
    for ( auto child : children )
        child->writeToFile(fileWrite, (char *) &writeInfo);

    fileWrite.close();
}