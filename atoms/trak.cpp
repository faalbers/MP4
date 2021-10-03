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

bool MP4::trak::isDataInSameFile() {
    for ( auto dref : getTypeAtoms<dref>() ) {
        for ( auto dataReference : dref->dataReferences ) {
            if ( dataReference-> key == "url " ) return ((url_ *) dataReference.get())->dataInSameFile;
            else if ( dataReference-> key == "alis" ) return ((alis *) dataReference.get())->dataInSameFile;
            else throw std::runtime_error("MP4::trak::isDataInSameFile Can't find data reference");
        }
    }
    return false;
}

std::map<uint32_t, MP4::stsdEntryType> MP4::trak::getSampleDescriptions()
{
    for ( auto stsd : getTypeAtoms<stsd>() )
        return stsd->stsdTable;
    throw std::runtime_error("MP4: no sample descriptions found !");
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
            chunkType chunk;
            chunk.ID = currentChunkID;
            chunk.trackID = getID();
            chunk.samples = stscTable.back().second[1];
            chunk.firstSampleID = sampleID;
            chunk.currentSampleID = sampleID;
            chunk.sampleDescriptionID = stscTable.back().second[2];
            chunk.dataOffset = chunkOffsets[currentChunkID];
            chunks.push_back(chunk);
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

bool MP4::trak::hasSampleDataFormat(std::string format)
{
    for ( auto entry : getSampleDescriptions() )
        if ( entry.second.dataFormat == format) return true;
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

std::string MP4::trak::key = "trak";
