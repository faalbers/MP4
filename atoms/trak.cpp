#include "trak.hpp"
#include "tkhd.hpp"
#include "mdia.hpp"
#include "tref.hpp"
#include "edts.hpp"
#include "elst.hpp"
#include "mdhd.hpp"
#include "hdlr.hpp"
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
#include "moov.hpp"
#include "mvhd.hpp"
#include "vmhd.hpp"
#include "ctts.hpp"

#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <string>

MP4::trak::trak(atomParse& parse)
    : atom(parse)
{
}

MP4::trak::trak(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;
    
    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<tkhd>(build);
    children_.push_back(child);
    
    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<tref>(build);
    children_.push_back(child);

    if ( build->getEditList().size() > 0 ) {
        build->setParentPath(parentPath_ + getKey() + "/");
        child = std::make_shared<edts>(build);
        children_.push_back(child);
    }
    
    build->setParentPath(parentPath_ + getKey() + "/");
    child = std::make_shared<mdia>(build);
    children_.push_back(child);
}

uint32_t MP4::trak::getID()
{
    for ( auto tkhd : getTypeAtoms<tkhd>() ) {
        if ( tkhd == nullptr ) error_("trak::getID: Track has no tkhd !");
        return tkhd->trackID;
    }
    error_("trak::getID: Track has no tkhd !");
    return 0;
}

uint32_t MP4::trak::getMediaTimeScale()
{
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        return mdhd->timeScale;
    }
    error_("trak::getMediaTimeScale: Track has no time scale !");
    return 0;
}

std::shared_ptr<MP4::trackType> MP4::trak::getTrack()
{
    auto trackData = std::make_shared<trackType>();
    uint32_t sampleID;
    
    // set default forcedd track ID to false
    trackData->enforcedTrackID = false;

    // can't handled data in referenced files
    if ( !isDataInSameFile() )
        error_("trak::getTrack: don't know how to handle referenced file");
    
    // get sample description
    for ( auto stsd : getTypeAtoms<stsd>() ) {
        if ( stsd->stsdTable.size() > 1 )
            error_("trak::getTrack: don't know how to handle multiple sample descriptions");
        trackData->dataFormat = stsd->stsdTable[1].dataFormat;
        trackData->dataReferenceIndex = stsd->stsdTable[1].dataReferenceIndex;
        trackData->dataExtended = stsd->stsdTable[1].dataExtended;
    }

    // get data references
    for ( auto dref : getTypeAtoms<dref>() ) {
        trackData->dataReferences = dref->getDataReferences();
    }

    // get edit list
    for ( auto elst : getTypeAtoms<elst>() ) {
        trackData->editList = elst->elstTable;
    }

    // get source trackID
    trackData->sourceTrackIDs[filePath_] = getID();

    // get tkhd data
    for ( auto tkhd : getTypeAtoms<tkhd>() ) {
        trackData->creationTime = tkhd->creationTime;
        trackData->modificationTime = tkhd->modificationTime;
        trackData->trackDuration = tkhd->duration;
        trackData->layer = tkhd->layer;
        trackData->width = tkhd->trackWidth;
        trackData->height = tkhd->trackHeight;
        trackData->volume = tkhd->volume;
        trackData->matrix = tkhd->matrix;
    }

    // get mdhd data
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        trackData->mediaTimeScale = mdhd->timeScale;
        trackData->mediaDuration = mdhd->duration;
        trackData->mediaLanguage = mdhd->language;
        trackData->mediaQuality = mdhd->quality;
    }

    // get hdlr data
    for ( auto hdlr : getTypeAtoms<hdlr>() ) {
        trackData->componentType = hdlr->componentType;
        trackData->componentSubType = hdlr->componentSubType;
        trackData->componentName = hdlr->componentName;
    }

    // get vmhd if it's a 'vide' track
    if ( trackData->componentSubType == "vide") {
        for ( auto vmhd : getTypeAtoms<vmhd>() ) {
            trackData->graphicsMode = vmhd->graphicsMode;
            trackData->opColorR = vmhd->opColorR;
            trackData->opColorG = vmhd->opColorG;
            trackData->opColorB = vmhd->opColorB;
        }
    }

    // video time scale and duration for reference
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>() ) {
        trackData->videoTimeScale = mvhd->timeScale;
        trackData->videoDuration = mvhd->duration;
    }

    // get referenced trackID's
    for ( auto tref : getTypeAtoms<tref>() ) {
        auto referenceTrackIDs = tref->getReferenceTrackIDs();
        if ( referenceTrackIDs.size() > 0 )
            trackData->sourceRefTrackIDs[filePath_] = referenceTrackIDs;
    }

    // get sync samples
    std::set<uint32_t> syncSamples;
    for ( auto stss : getTypeAtoms<stss>() ) {
        for ( auto syncID : stss->stssTable ) syncSamples.insert(syncID.second);
    }

    // greate samples from time-to-sample and set time and durations
    sampleID = 0;
    sampleType sample;
    uint32_t time = 0;
    for ( auto stts : getTypeAtoms<stts>() ) {
        for ( auto entry : stts->sttsTable ) {
            uint32_t index = 0;
            do {
                sampleID++;
                sample.filePath = filePath_;
                sample.sync = false;
                if ( syncSamples.find(sampleID) != syncSamples.end()) sample.sync = true;
                sample.time = time;
                sample.duration = entry.second[1];
                sample.compositionOffset = 0;
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

    // get chunks and set file positions and chunk ID's for samples
    int64_t filePos = 0;
    for ( auto chunk : getChunks() ) {
        if ( chunk.second.sampleDescriptionID > 1 )
            error_("trak::getTrack: don't know how to handle multiple sample descriptions");
        if (filePos > (int64_t) chunk.second.dataOffset )
            error_("trak::getTrack: wrong file position on samples");
        auto lastChunkSampleID = chunk.second.firstSampleID + chunk.second.samples - 1;
        filePos = chunk.second.dataOffset;
        for ( auto sampleID = chunk.second.firstSampleID; sampleID <= lastChunkSampleID; sampleID++ ) {
            trackData->samples[sampleID].filePos = filePos;
            trackData->samples[sampleID].chunkID = chunk.first;
            filePos += trackData->samples[sampleID].size;
        }
    }

    // set composition offsets
    sampleID = 1;
    for ( auto ctts : getTypeAtoms<ctts>() ) {
        for ( auto entry : ctts->cttsTable ) {
            for ( uint32_t sampleCount = 0; sampleCount < entry.second[0]; sampleCount++ ) {
                trackData->samples[sampleID].compositionOffset = entry.second[1];
                sampleID++;
            }
        }
    }

    return trackData;
}

bool MP4::trak::isDataInSameFile() {
    for ( auto stsd : getTypeAtoms<stsd>() ) {
        for ( auto refIndex : stsd->getReferenceIndices() ) {
            for ( auto dref : getTypeAtoms<dref>() ) {
                return dref->isDataInSameFile(refIndex.second);
            }
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

std::map<uint32_t, MP4::chunkType> MP4::trak::getChunks()
{
    std::map<uint32_t, chunkType> chunks;
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
            error_("trak::getChunks: No chunk offsets found !");
        auto stscTableMap = stsc->stscTable;

        // reverse vectors so we can pop first elements from back
        std::vector<std::pair<uint32_t, std::vector<uint32_t>>> stscTable;
        for ( auto stscEntry : stscTableMap ) stscTable.push_back(stscEntry);
        std::reverse(stscTable.begin(),stscTable.end());
        uint32_t currentChunkID = stscTable.back().second[0];

        do {
            chunkType chunk;
            chunk.samples = stscTable.back().second[1];
            chunk.firstSampleID = sampleID;
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
    error_("trak::getChunkOffsets: no Chunks found in track !");
    std::map<uint32_t, uint64_t> empty;
    return empty;
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

void MP4::trak::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

std::string MP4::trak::getKey() const
{
    return key;
}

const std::string MP4::trak::key = "trak";
