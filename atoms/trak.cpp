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

std::vector<uint32_t> MP4::trak::getSampleAtTime(float sampleTime)
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
                        std::vector<uint32_t> returnValue;
                        returnValue.push_back(sampleID);
                        returnValue.push_back(entry[1]);
                        returnValue.push_back(currentStart);
                        returnValue.push_back(time - currentStart);

                        return returnValue;
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

std::vector<std::vector<uint32_t>>  MP4::trak::getSamples()
{
    std::vector<std::vector<uint32_t>> samples;

    for ( auto stts : getTypeAtoms<stts>() ) {
        uint32_t sampleID = 1;
        uint32_t currentStart = 0;
        for ( auto entry : stts->sttsTable ) {
            for ( uint32_t sttsSampleID = 1; sttsSampleID <= entry[0]; sttsSampleID++) {
                // add a sample
                std::vector<uint32_t> sample;
                sample.push_back(sampleID);
                sample.push_back(entry[1]);
                sample.push_back(currentStart);
                sample.push_back(0);
                samples.push_back(sample);
                currentStart += entry[1];
                sampleID++;
            }
        }
    }
    return samples;
}

uint32_t MP4::trak::getChunkOffsetsSize()
{
    for ( auto stco : getTypeAtoms<stco>() ) {
        return stco->stcoTable.size();
    }
    for ( auto co64 : getTypeAtoms<co64>() ) {
        return co64->co64Table.size();
    }
    return 0;
}

std::vector<uint32_t> MP4::trak::getSampleToChunk(std::vector<uint32_t> sample)
{
    std::vector<uint32_t> sampleToChunk;
    for ( auto stsc : getTypeAtoms<stsc>() ) {
        uint32_t topSampleID = 1;
        uint32_t chunksSize = getChunkOffsetsSize();
        auto stscTable = stsc->stscTable;
        
        // reverse vectors so we can pop first elements from back
        std::reverse(stscTable.begin(),stscTable.end());
        uint32_t currentChunkID = stscTable.back()[0];

        do {
            topSampleID += stscTable.back()[1];
            if ( sample[0] < topSampleID ) {
                sampleToChunk.push_back(currentChunkID);
                sampleToChunk.push_back(stscTable.back()[1]);
                sampleToChunk.push_back(sample[0] - (topSampleID - stscTable.back()[1]) + 1);
                sampleToChunk.push_back(stscTable.back()[2]);
                return sampleToChunk;
            }
            currentChunkID++;
            // check next first chunk if available and pop if we get there
            if ( stscTable.size() >= 2 && stscTable[stscTable.size()-2][0] == currentChunkID )
                    stscTable.pop_back();
        } while ( currentChunkID <= chunksSize );
    }
    throw std::runtime_error("MP4: no Chunk found for Sample !");
}

std::vector<std::vector<uint32_t>> MP4::trak::getChunks()
{
    std::vector<std::vector<uint32_t>> samplesToChunk;

    for ( auto stsc : getTypeAtoms<stsc>() ) {
        uint32_t sampleID = 1;
        uint32_t chunksSize = getChunkOffsetsSize();
        auto stscTable = stsc->stscTable;
        
        // reverse vectors so we can pop first elements from back
        std::reverse(stscTable.begin(),stscTable.end());
        uint32_t currentChunkID = stscTable.back()[0];

        do {
            std::cout << "[" << currentChunkID << "] ( " << stscTable.back()[1]
                << ", " << sampleID
                << ", " << stscTable.back()[2] << " )\n";
            sampleID += stscTable.back()[1];
            currentChunkID++;
            // check next first chunk if available and pop if we get there
            if ( stscTable.size() >= 2 && stscTable[stscTable.size()-2][0] == currentChunkID )
                    stscTable.pop_back();
        } while ( currentChunkID <= chunksSize );
    }

    return samplesToChunk;
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

