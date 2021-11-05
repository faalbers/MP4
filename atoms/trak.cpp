#include "trak.hpp"
#include "tkhd.hpp"
#include "mdhd.hpp"
#include "dref.hpp"
#include "url_.hpp"
#include "alis.hpp"
#include "stss.hpp"
#include "stts.hpp"
#include "stsz.hpp"
#include "stsd.hpp"
#include "stsc.hpp"
#include "stco.hpp"
#include "co64.hpp"
#include "hdlr.hpp"

#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <string>

MP4::trak::trak(atomParse &parse)
    : atom(parse)
{
}

MP4::trak::trak(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    headerSize_ = 8;
    path_ = parentPath_ + key;

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

std::shared_ptr<MP4::trackType> MP4::trak::getTrack()
{
    auto trackData = std::make_shared<trackType>();
    
    // get data references
    std::string filePath = "";
    for ( auto dref : getTypeAtoms<dref>() ) {
        if ( dref->dataReferences.size() > 1 )
            throw std::runtime_error("MP4::getSamples: don't know how to handle multiple data references");
        if ( dref->dataReferences[1]->getKey() =="url " && ((url_ *) dref->dataReferences[1].get())->dataInSameFile )
            filePath = filePath_;
        else if ( dref->dataReferences[1]->getKey() =="alis" && ((alis *) dref->dataReferences[1].get())->dataInSameFile )
            filePath = filePath_;
        else {
            throw std::runtime_error("MP4::getSamples: Can't handle reference of type: "+dref->dataReferences[1]->getKey());
        }
    }

    // get sample description
    for ( auto stsd : getTypeAtoms<stsd>() ) {
        if ( stsd->stsdTable.size() > 1 )
            throw std::runtime_error("MP4::getSamples: don't know how to handle multiple sample descriptions");
        trackData->dataFormat = stsd->stsdTable[1].dataFormat;
    }

    // get trackID
    trackData->trackID = getID();

    // get track time scale and duration
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        trackData->mediaTimeScale = mdhd->timeScale;
        trackData->mediaDuration = mdhd->duration;
    }

    // get sync samples
    std::set<uint32_t> syncSamples;
    for ( auto stss : getTypeAtoms<stss>() ) {
        for ( auto syncID : stss->stssTable ) syncSamples.insert(syncID.second);
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
                sample.filePath = filePath;
                sample.sync = false;
                if ( syncSamples.find(sampleID) != syncSamples.end()) sample.sync = true;
                sample.time = time;
                sample.duration = entry.second[1];
                trackData->samples[sampleID] = sample;
                time += sample.duration;
                index++;
            } while ( index < entry.second[0] );
        }
    }
    trackData->samplesDuration =  time;

    // set sample data sizes
    for ( auto stsz : getTypeAtoms<stsz>()) {
        if ( stsz->defaultSampleSize == 0 ) {
            uint32_t sampleID = 1;
            for ( auto entry : stsz->stszTable )
                trackData->samples[entry.first].size = entry.second;
            /*
            sampleID--;
            if ( sampleID != trackData->sampleCount )
                throw std::runtime_error("MP4::getSamples: wrong sample count: "+std::to_string(sampleID));
            */
        } else {
            for ( auto sample : trackData->samples )
                trackData->samples[sample.first].size = stsz->defaultSampleSize;
        }
    }

    // get chunks and set file positions for samples
    int64_t filePos = 0;
    for ( auto chunk : _getChunks() ) {
        if ( chunk.second.sampleDescriptionID > 1 )
            throw std::runtime_error("MP4::getSamples: don't know how to handle multiple sample descriptions");
        if (filePos > (int64_t) chunk.second.dataOffset )
            throw std::runtime_error("MP4::getSamples: wrong file position on samples");
        auto lastChunkSampleID = chunk.second.firstSampleID + chunk.second.samples - 1;
        filePos = chunk.second.dataOffset;
        for ( auto sampleID = chunk.second.firstSampleID; sampleID <= lastChunkSampleID; sampleID++ ) {
            trackData->samples[sampleID].filePos = filePos;
            filePos += trackData->samples[sampleID].size;
        }
    }

    return trackData;
}

bool MP4::trak::isDataInSameFile() {
    for ( auto dref : getTypeAtoms<dref>() ) {
        for ( auto dataReference : dref->dataReferences ) {
            if ( dataReference.second->getKey() == "url " ) return ((url_ *) dataReference.second.get())->dataInSameFile;
            else if ( dataReference.second->getKey() == "alis" ) return ((alis *) dataReference.second.get())->dataInSameFile;
            else throw std::runtime_error("MP4::trak::isDataInSameFile Can't find data reference");
        }
    }
    return false;
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

std::map<uint32_t, MP4::trak::_chunkType> MP4::trak::_getChunks()
{
    std::map<uint32_t, _chunkType> chunks;
    for ( auto stsc : getTypeAtoms<stsc>() ) {
        uint32_t sampleID = 1;
        std::map<uint32_t, uint64_t> chunkOffsets;
        for ( auto stco : getTypeAtoms<stco>() ) {
            chunkOffsets = stco->stcoTable;
        }
        for ( auto co64 : getTypeAtoms<co64>() ) {
            chunkOffsets = co64->co64Table;
        }
        if ( chunkOffsets.size() == 0 )
            throw std::runtime_error("MP4::trak: No chunk offsets found !");
        auto stscTableMap = stsc->stscTable;

        // reverse vectors so we can pop first elements from back
        std::vector<std::pair<uint32_t, std::vector<uint32_t>>> stscTable;
        for ( auto stscEntry : stscTableMap ) stscTable.push_back(stscEntry);
        std::reverse(stscTable.begin(),stscTable.end());
        uint32_t currentChunkID = stscTable.back().second[0];

        do {
            _chunkType chunk;
            chunk.samples = stscTable.back().second[1];
            chunk.firstSampleID = sampleID;
            chunk.currentSampleID = sampleID;
            chunk.sampleDescriptionID = stscTable.back().second[2];
            chunk.dataOffset = chunkOffsets[currentChunkID];
            chunks[currentChunkID] = chunk;
            sampleID += stscTable.back().second[1];
            currentChunkID++;
            // check next first chunk if available and pop if we get there
            if ( stscTable.size() >= 2 && stscTable[stscTable.size()-2].second[0] == currentChunkID )
                    stscTable.pop_back();
        } while ( currentChunkID <= chunkOffsets.size() );
    }

    return chunks;
}

std::map<uint32_t, uint64_t> MP4::trak::getChunkOffsets()
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

bool MP4::trak::isDataFormat(std::string dataFormat)
{
    for ( auto stsd : getTypeAtoms<stsd>() )
        for ( auto entry : stsd->stsdTable )
            if ( entry.second.dataFormat == dataFormat ) return true;
    return false;
}

void MP4::trak::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::trak::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::trak::getKey()
{
    return key;
}

std::string MP4::trak::key = "trak";
