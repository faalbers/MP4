#include "trak.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

MP4::trak::trak(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
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

std::vector<std::shared_ptr<MP4::atom>> MP4::trak::getDataReferences()
{
    for ( auto dref : getTypeAtoms<dref>() )
        return dref->dataReferences;
    throw std::runtime_error("MP4: SampleData has no data references !");
}

std::shared_ptr<MP4::atom> MP4::trak::getDataReference(std::string dataFormat)
{
    for ( auto sampleDesc : getSampleDescriptions() ) {
        if ( sampleDesc.dataFormat == dataFormat ) {
            for ( auto dref : getTypeAtoms<dref>() )
                if ( dref->dataReferences.size() <= sampleDesc.dataReferenceIndex ) {
                    return dref->dataReferences[sampleDesc.dataReferenceIndex-1];
                }
        }
    }
    throw std::runtime_error("MP4: Samples have no data reference of that format !");
}

MP4::sampleType MP4::trak::getSampleAtTime(float sampleTime)
{
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        auto timeScale = mdhd->timeScale;
        uint32_t time = (uint32_t) ((float) timeScale * sampleTime);
        for ( auto sample : getSamples().samples) {
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

MP4::samplesType MP4::trak::getSamples()
{
    samplesType samples;

    // get data references
    samples.filePath = "";
    for ( auto dref : getTypeAtoms<dref>() ) {
        if ( dref->dataReferences.size() > 1 )
            throw std::runtime_error("MP4::getSamples: don't know how to handle multiple data references");
        if ( dref->dataReferences[0]->key =="url " && ((url_ *) dref->dataReferences[0].get())->dataInSameFile )
            samples.filePath = filePath_;
        if ( dref->dataReferences[0]->key =="alis" && ((alis *) dref->dataReferences[0].get())->dataInSameFile )
            samples.filePath = filePath_;
    }

    // get sample description
    std::map<uint32_t, std::vector<std::string>> sampleDescriptions;
    for ( auto stsd : getTypeAtoms<stsd>() ) {
        if ( stsd->stsdTable.size() > 1 )
            throw std::runtime_error("MP4::getSamples: don't know how to handle multiple sample descriptions");
        samples.dataFormat = stsd->stsdTable[0].dataFormat;
    }    

    // get trackID
    samples.trackID = getID();

    // get track time scale and duration
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        samples.mediaTimeScale = mdhd->timeScale;
        samples.mediaDuration = mdhd->duration;
    }
    
    // greate samples from time-to-sample and set time and durations
    uint32_t sampleID = 0;
    sampleType sample;
    uint32_t time = 0;
    for ( auto stts : getTypeAtoms<stts>() ) {
        for ( auto entry : stts->sttsTable ) {
            uint32_t index = 0;
            do {
                sampleID++;
                sample.ID = sampleID;
                sample.filePath =  samples.filePath;
                sample.trackID = samples.trackID;
                sample.time = time;
                sample.duration = entry[1];
                samples.samples.push_back(sample);
                time += sample.duration;
                index++;
            } while ( index < entry[0] );
        }
    }
    samples.samplesDuration =  time;
    samples.sampleCount = samples.samples.size();


    // set sample data sizes
    for ( auto stsz : getTypeAtoms<stsz>()) {
        if ( stsz->defaultSampleSize == 0 ) {
            uint32_t sampleID = 1;
            for ( auto entry : stsz->stszTable ) {
                samples.samples[sampleID-1].size = entry;
                sampleID++;
            }
            sampleID--;
            if ( sampleID != samples.sampleCount )
                throw std::runtime_error("MP4::getSamples: wrong sample count: "+std::to_string(sampleID));
        } else {
            for ( auto sample : samples.samples )
                sample.size = stsz->defaultSampleSize;
        }
    }

    // get chunks and set file path and file positions for samples
    int64_t filePos = 0;
    for ( auto chunk : getChunks() ) {
        if ( chunk.sampleDescriptionID > 1 )
            throw std::runtime_error("MP4::getSamples: don't know how to handle multiple sample descriptions");
        if (filePos > chunk.dataOffset )
            throw std::runtime_error("MP4::getSamples: wrong file position on samples");
        auto lastChunkSampleID = chunk.firstSampleID + chunk.samples - 1;
        filePos = chunk.dataOffset;
        for ( auto sampleID = chunk.firstSampleID; sampleID <= lastChunkSampleID; sampleID++ ) {
            samples.samples[sampleID-1].filePos = filePos;
            filePos +=samples.samples[sampleID-1].size;
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

        // reverse vectors so we can pop first elements from back
        std::reverse(stscTable.begin(),stscTable.end());
        uint32_t currentChunkID = stscTable.back()[0];

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
