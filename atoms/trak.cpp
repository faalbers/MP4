#include "trak.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <algorithm>

MP4::trak::trak(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
}

uint32_t MP4::trak::getID()
{
    for ( auto tkhd : getTypeAtoms<tkhd>() ) {
        if ( tkhd == nullptr ) throw std::runtime_error("MP4: Track has no tkhd !");
        return tkhd->trackID;
    }
    throw std::runtime_error("MP4: Track has no tkhd !");
}

std::vector<std::string> MP4::trak::getSampleDataFormats()
{
    std::vector<std::string> dataFormats;
    for ( auto stsd : getTypeAtoms<stsd>() )
        for ( auto entry : stsd->stsdTable ) dataFormats.push_back(entry.dataFormat);
    return dataFormats;
}

std::string MP4::trak::getSampleDataReference(std::string dataFormat)
{
    for ( auto stsd : getTypeAtoms<stsd>() ) {
        for ( auto entry : stsd->stsdTable ) {
            if ( entry.dataFormat == dataFormat )
                for ( auto dref : getTypeAtoms<dref>() )
                    return dref->drefTable[entry.dataReferenceIndex - 1];
        }
    }
    throw std::runtime_error("MP4: SampleData has no reference !");
}

MP4::sampleType MP4::trak::getSampleAtTime(float sampleTime)
{
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        auto timeScale = mdhd->timeScale;
        uint32_t time = timeScale * sampleTime;
        for ( auto stts : getTypeAtoms<stts>() ) {
            uint32_t nextStart = 0;
            uint32_t currentStart = 0;
            uint32_t sampleID = 1;
            for ( auto entry : stts->sttsTable ) {
                for ( uint32_t sttsSampleID = 1; sttsSampleID <= entry[0]; sttsSampleID++) {
                    nextStart += entry[1];
                    if ( nextStart > time ) {
                        sampleType sample;
                        sample.ID = sampleID;
                        sample.duration = entry[1];
                        sample.time = currentStart;
                        sample.timeOffset = time - currentStart;
                        return sample;
                    }
                    currentStart = nextStart;
                    sampleID++;
                }
            }
            throw std::runtime_error("MP4: Sample Index not found or out of range !");
        }
    }
    throw std::runtime_error("MP4: Sample Index not found or out of range !");
}

std::vector<MP4::sampleType>  MP4::trak::getSamples()
{
    //(ID, duration, time, timeOffset)
    std::vector<sampleType> samples;

    for ( auto stts : getTypeAtoms<stts>() ) {
        uint32_t sampleID = 1;
        uint32_t currentStart = 0;
        for ( auto entry : stts->sttsTable ) {
            for ( uint32_t sttsSampleID = 1; sttsSampleID <= entry[0]; sttsSampleID++) {
                // add a sample
                sampleType sample;
                sample.ID = sampleID;
                sample.duration = entry[1];
                sample.time = currentStart;
                sample.timeOffset = 0;
                samples.push_back(sample);
                currentStart += entry[1];
                sampleID++;
            }
        }
    }
    return samples;
}

uint32_t MP4::trak::getChunksSize()
{
    for ( auto stco : getTypeAtoms<stco>() ) {
        return stco->stcoTable.size();
    }
    for ( auto co64 : getTypeAtoms<co64>() ) {
        return co64->co64Table.size();
    }
    return 0;
}

MP4::chunkOffsetType MP4::trak::sampleToChunk(sampleType sample)
{
    chunkOffsetType sampleToChunk;
    for ( auto chunk : getChunks() ) {
        if ( sample.ID < chunk.samples + chunk.ID ) {
            uint32_t sampleOffset = sample.ID-chunk.samples + 1;
            sampleToChunk.ID = chunk.ID;
            sampleToChunk.sampleOffset = sampleOffset;
            return sampleToChunk;
        }
    }
    /*
    for ( auto chunk : getChunks() ) {
        if ( sample.ID < chunk[2]+chunk[1] ) {
            uint32_t sampleOffset = sample.ID-chunk[2]+1;
            sampleToChunk.push_back(chunk[0]);
            sampleToChunk.push_back(sampleOffset);
            return sampleToChunk;
        }
    }
    */
    throw std::runtime_error("MP4: no Chunk found for Sample !");
}

std::vector<MP4::chunkType> MP4::trak::getChunks()
{
    std::vector<chunkType> chunks;
    for ( auto stsc : getTypeAtoms<stsc>() ) {
        uint32_t sampleID = 1;
        uint32_t chunksSize = getChunksSize();
        auto stscTable = stsc->stscTable;
        
        // test setup
        //sampleID[0] = 5;
        /*
        std::vector<std::vector<uint32_t>> stscTable;
        std::vector<uint32_t> stscLine;
        stscLine.push_back(1); stscLine.push_back(3); stscLine.push_back(23); 
        stscTable.push_back(stscLine);
        stscLine[0] = 3;stscLine[1] = 1;stscLine[2] = 23;
        stscTable.push_back(stscLine);
        stscLine[0] = 5;stscLine[1] = 1;stscLine[2] = 24;
        stscTable.push_back(stscLine);
        */

        // reverse vectors so we can pop first elements from back
        std::reverse(stscTable.begin(),stscTable.end());
        uint32_t currentChunkID = stscTable.back()[0];

        // enter starting chunks if first entry of stsc does not start with 1
        for ( uint32_t startChunk = 1; startChunk < currentChunkID; startChunk++ ) {
            chunkType chunk;
            chunk.ID = startChunk;
            chunk.samples = 0;
            chunk.firstSampleID = 0;
            chunk.sampleDescriptionID = 0;
            chunk.dataOffset = 0;
            chunks.push_back(chunk);
        }

        do {
            chunkType chunk;
            chunk.ID = currentChunkID;
            chunk.samples = stscTable.back()[1];
            chunk.firstSampleID = sampleID;
            chunk.sampleDescriptionID = stscTable.back()[2];
            chunk.dataOffset = 0;
            chunks.push_back(chunk);
            
            sampleID += stscTable.back()[1];
            currentChunkID++;
            // check next first chunk if available and pop if we get there
            if ( stscTable.size() >= 2 && stscTable[stscTable.size()-2][0] == currentChunkID )
                    stscTable.pop_back();
        } while ( currentChunkID <= chunksSize );
    }

    return chunks;
}

std::vector<uint64_t> MP4::trak::getChunkOffsets()
{
    for ( auto stco : getTypeAtoms<stco>() ) {
        return stco->stcoTable;
    }
    for ( auto co64 : getTypeAtoms<co64>() ) {
        return co64->co64Table;
    }
    throw std::runtime_error("MP4: no Chunks found in track !");
}

bool MP4::trak::isComponentType(std::string type)
{
    for ( auto hdlr : getTypeAtoms<hdlr>() )
        if ( hdlr->componentType == type) return true;
    return false;
}

bool MP4::trak::isComponentSubType(std::string type)
{
    for ( auto hdlr : getTypeAtoms<hdlr>() )
        if ( hdlr->componentSubType == type) return true;
    return false;
}

bool MP4::trak::hasSampleDataFormat(std::string format)
{
    for ( auto stsd : getTypeAtoms<stsd>() ) {
        for ( auto entry : stsd->stsdTable )
            if ( entry.dataFormat == format) return true;
    }
    return false;
}

void MP4::trak::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::trak::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::trak::key = "trak";

