#include "trak.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <algorithm>

MP4::trak::trak(internal::atomBuildType &atomBuild, uint64_t filePos)
    : atom(atomBuild, filePos)
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

uint32_t MP4::trak::getMediaTimeScale()
{
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        return mdhd->timeScale;
    }
    throw std::runtime_error("MP4: Track has no time scale !");
}

std::vector<MP4::drefEntryType> MP4::trak::getDataReferences()
{
    for ( auto dref : getTypeAtoms<dref>() )
        return dref->drefTable;
    throw std::runtime_error("MP4: SampleData has no data references !");
}

MP4::drefEntryType MP4::trak::getDataReference(std::string dataFormat)
{
    for ( auto sampleDesc : getSampleDescriptions() ) {
        if ( sampleDesc.dataFormat == dataFormat ) {
            for ( auto dref : getTypeAtoms<dref>() )
                for ( auto drefEntry : dref->drefTable )
                    if ( drefEntry.ID == sampleDesc.dataReferenceIndex )
                        return drefEntry;
        }
    }
    throw std::runtime_error("MP4: Samples have no data reference of that format !");
}

MP4::sampleType MP4::trak::getSampleAtTime(float sampleTime)
{
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        auto timeScale = mdhd->timeScale;
        uint32_t time = (uint32_t) ((float) timeScale * sampleTime);
        for ( auto sample : getSamples()) {
            if ( time < (sample.time + sample.duration) ) {
                sample.currentTime = time;
                return sample;
            }
        }
    }
    throw std::runtime_error("MP4: No sample found at sampleTime !");
}

size_t MP4::trak::getSampleCount()
{
    for ( auto stsz : getTypeAtoms<stsz>() ) {
        return stsz->stszTable.size();
    }
    return 0;
}

std::vector<MP4::sampleType>  MP4::trak::getSamples()
{
    std::vector<sampleType> samples;
    uint32_t                timeScale;
    for ( auto mdhd : getTypeAtoms<mdhd>() )
        timeScale = mdhd->timeScale;
    for ( auto stts : getTypeAtoms<stts>() ) {
        uint32_t sampleID = 1;
        uint32_t currentStart = 0;
        uint32_t defaultSampleSize;               
        std::vector<uint32_t>   sampleSizes;
        for ( auto stsz : getTypeAtoms<stsz>() ) {
            defaultSampleSize = stsz->defaultSampleSize;
            sampleSizes = stsz->stszTable;
        }
        for ( auto entry : stts->sttsTable ) {
            for ( uint32_t sttsSampleID = 1; sttsSampleID <= entry[0]; sttsSampleID++) {
                // add a sample
                sampleType sample;
                sample.ID = sampleID;
                sample.duration = entry[1];
                sample.time = currentStart;
                sample.currentTime = currentStart;
                sample.timeScale = timeScale;
                if ( defaultSampleSize != 0 )
                    sample.dataSize = defaultSampleSize;
                else
                    sample.dataSize = sampleSizes[sampleID - 1];
                samples.push_back(sample);
                currentStart += entry[1];
                sampleID++;
            }
        }
    }
    return samples;
}

std::vector<MP4::stsdEntryType> MP4::trak::getSampleDescriptions()
{
    for ( auto stsd : getTypeAtoms<stsd>() )
        return stsd->stsdTable;
    throw std::runtime_error("MP4: no sample descriptions found !");
}

MP4::chunkType MP4::trak::sampleToChunk(sampleType sample)
{
    for ( auto chunk : getChunks() ) {
        if ( sample.ID < (chunk.firstSampleID + chunk.samples) ) {
            chunk.currentSampleID = sample.ID;
            return chunk;
        }
    }
    throw std::runtime_error("MP4: no Chunk found for Sample !");
}

size_t MP4::trak::getChunkCount()
{
    for ( auto stco : getTypeAtoms<stco>() ) {
        return (size_t) stco->stcoTable.size();
    }
    for ( auto co64 : getTypeAtoms<co64>() ) {
        return (size_t) co64->co64Table.size();
    }
    return 0;
}

std::vector<MP4::chunkType> MP4::trak::getChunks()
{
    std::vector<chunkType> chunks;
    for ( auto stsc : getTypeAtoms<stsc>() ) {
        uint32_t sampleID = 1;
        std::vector<uint64_t> chunkOffsets;
        for ( auto stco : getTypeAtoms<stco>() ) {
            chunkOffsets = stco->stcoTable;
        }
        for ( auto co64 : getTypeAtoms<co64>() ) {
            chunkOffsets = co64->co64Table;
        }
        if ( chunkOffsets.size() == 0 )
            throw std::runtime_error("MP4::trak: No chunk offsets found !");
        auto stscTable = stsc->stscTable;

        // test setup
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
            chunk.trackID = getID();
            chunk.samples = 0;
            chunk.firstSampleID = 0;
            chunk.currentSampleID = 0;
            chunk.sampleDescriptionID = 0;
            chunk.dataOffset = 0;
            chunks.push_back(chunk);
        }

        do {
            chunkType chunk;
            chunk.ID = currentChunkID;
            chunk.trackID = getID();
            chunk.samples = stscTable.back()[1];
            chunk.firstSampleID = sampleID;
            chunk.currentSampleID = sampleID;
            chunk.sampleDescriptionID = stscTable.back()[2];
            chunk.dataOffset = chunkOffsets[currentChunkID-1];
            chunks.push_back(chunk);
            sampleID += stscTable.back()[1];
            currentChunkID++;
            // check next first chunk if available and pop if we get there
            if ( stscTable.size() >= 2 && stscTable[stscTable.size()-2][0] == currentChunkID )
                    stscTable.pop_back();
        } while ( currentChunkID <= chunkOffsets.size() );
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
    for ( auto entry : getSampleDescriptions() )
        if ( entry.dataFormat == format) return true;
    return false;
}

void MP4::trak::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
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
