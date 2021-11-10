#include "Parser.hpp"
#include <filesystem>
#include <memory>
#include "atoms/atom.hpp"
#include "atoms/trak.hpp"
#include "atoms/stsd.hpp"
#include "atoms/hdlr.hpp"
#include <iostream>

#include "atoms/stsc.hpp"

MP4::Parser::Parser(std::string fileName)
{
    try {
        atomParse parse(fileName);
        filePath_ = parse.getFilePath();
        rootAtom_ = std::make_shared<root>(parse);
    } catch (std::runtime_error &error) {
        error_(error.what());
    }
}

void MP4::Parser::printHierarchy()
{
    int pathWith = rootAtom_->nestLevel_(0) * 5;
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

    rootAtom_->printHierarchy(pathWith, valWith);
    
    std::cout << std::endl;
}

void MP4::Parser::printHierarchyData(bool fullLists)
{
    //std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    std::cout << std::endl;
    rootAtom_->printHierarchyData(fullLists);
}

void MP4::Parser::printMdatMap()
{
    std::map<uint64_t, std::vector<uint32_t>> mdatMap;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        auto track = trak->getTrack();
        for ( auto chunk : trak->getChunks() ) {
            std::vector<uint32_t> entry;
            entry.push_back(trak->getID());
            entry.push_back(chunk.first);
            entry.push_back(chunk.second.firstSampleID);
            entry.push_back(chunk.second.samples);

            auto sampleTime = atom::timeScaleDuration(
                track->samples[chunk.second.firstSampleID].time,
                track->mediaTimeScale, track->videoTimeScale);
            uint32_t chunkDuration = 0;
            uint32_t chunkSize = 0;
            for ( uint32_t sampleOffsetID = 0; sampleOffsetID < chunk.second.samples; sampleOffsetID++ ) {
                auto sampleID = chunk.second.firstSampleID+sampleOffsetID;
                chunkDuration += track->samples[sampleID].duration;
                chunkSize += track->samples[sampleID].size;
            }
            chunkDuration = atom::timeScaleDuration(
                chunkDuration,
                track->mediaTimeScale, track->videoTimeScale);

            entry.push_back(chunkSize);
            entry.push_back(track->videoTimeScale);
            entry.push_back(sampleTime);
            entry.push_back(chunkDuration);
            mdatMap[chunk.second.dataOffset] = entry;
        }
    }
    std::cout << std::string(82, '-') << std::endl;
    std::cout << "| trackID | chunkID | sampleID | sCount |   size |          time |      duration |\n";
    std::cout << std::string(82, '-') << std::endl;
    for ( auto entry : mdatMap ) {
        std::cout << "|" <<  std::setw(8) << std::right << entry.second[0]
            << " |" << std::setw(8) << entry.second[1]
            << " |" << std::setw(9) << entry.second[2]
            << " |" << std::setw(7) << entry.second[3]
            << " |" << std::setw(7) << entry.second[4]
            << " |" << std::setw(14) << atom::getTimeString(entry.second[6], entry.second[5])
            << " |" << std::setw(14) << atom::getTimeString(entry.second[7], entry.second[5]) << " |\n";
    }

    // chunk timing analysis
    /*
    std::map<uint64_t, std::vector<uint32_t>> mdatMap;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        auto track = trak->getTrack();
        for ( auto chunk : trak->getChunks() ) {
            std::vector<uint32_t> entry;
            entry.push_back(trak->getID());
            entry.push_back(chunk.first);
            auto sampleTime = atom::timeScaleDuration(
                track->samples[chunk.second.firstSampleID].time,
                track->mediaTimeScale, track->videoTimeScale);
            entry.push_back(sampleTime);
            uint32_t chunkDuration = 0;
            for ( uint32_t sampleOffsetID = 0; sampleOffsetID < chunk.second.samples; sampleOffsetID++ ) {
                chunkDuration += track->samples[chunk.second.firstSampleID+sampleOffsetID].duration;
            }
            chunkDuration = atom::timeScaleDuration(
                chunkDuration,
                track->mediaTimeScale, track->videoTimeScale);
            entry.push_back(chunkDuration);
            mdatMap[chunk.second.dataOffset] = entry;
        }
    }
    for ( auto entry : mdatMap ) {
        std::cout << entry.second[0]
            << " " << entry.second[1]
            << " " << entry.second[2]
            << " " << entry.second[3]
            << std::endl;
    }
    */

    // sample timing analysis
    /*
    std::map<uint64_t, std::vector<uint32_t>> mdatMap;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        auto trackID = trak->getID();
        if ( !(trackID == 1 || trackID == 2 || trackID == 4) ) continue;
        auto track = trak->getTrack();
        for ( auto sample : track->samples ) {
            std::vector<uint32_t> entry;
            auto sampleTime = atom::timeScaleDuration(
                sample.second.time, track->mediaTimeScale, track->videoTimeScale);
            auto sampleDuration = atom::timeScaleDuration(
                sample.second.duration, track->mediaTimeScale, track->videoTimeScale);
            entry.push_back(track->trackID);
            entry.push_back(sample.second.chunkID);
            entry.push_back(sampleTime);
            entry.push_back(sampleTime+sampleDuration);
            entry.push_back(sampleDuration);
            entry.push_back(sample.second.size);
            sample.second.
            mdatMap[sample.second.filePos] = entry;
        }
    }
    for ( auto entry : mdatMap ) {
        std::cout
            << " trackID: " << entry.second[0]
            << " chunkID: " << entry.second[1]
            << " sampleTime: " << entry.second[2]
            << " sampleEndTime: " << entry.second[3]
            << " sampleDur: " << entry.second[4]
            << " sampleSize: " << entry.second[5]
            << std::endl;
    }
    */

}

std::set<std::string> MP4::Parser::getDataFormats()
{
    std::set<std::string> dataFormats;
    for ( auto stsd : rootAtom_->getTypeAtoms<stsd>() ) {
        for ( auto dataFormat : stsd->getDataFormats() )
            dataFormats.insert(dataFormat);
    }
    return dataFormats;
}

std::set<std::string> MP4::Parser::getComponentTypes()
{
    std::set<std::string> componentSubTypes;
    for ( auto hdlr : rootAtom_->getTypeAtoms<hdlr>() )
        componentSubTypes.insert(hdlr->componentType);
    return componentSubTypes;
}

std::set<std::string> MP4::Parser::getComponentSubTypes()
{
    std::set<std::string> componentSubTypes;
    for ( auto hdlr : rootAtom_->getTypeAtoms<hdlr>() )
        componentSubTypes.insert(hdlr->componentSubType);
    return componentSubTypes;
}

std::set<uint32_t> MP4::Parser::getDataFormatTrackIDs(std::string dataFormat)
{
    std::set<uint32_t> trackIDs;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        if ( dataFormat != "" && !trak->isDataFormat(dataFormat) ) continue;
        trackIDs.insert(trak->getID());
    }
    return trackIDs;
}

std::set<uint32_t> MP4::Parser::getComponentSubTypeTrackIDs(std::string componentSubType)
{
    std::set<uint32_t> trackIDs;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        if ( componentSubType != "" && !trak->isComponentSubType(componentSubType) ) continue;
        trackIDs.insert(trak->getID());
    }
    return trackIDs;
}

std::shared_ptr<MP4::trackType> MP4::Parser::getTrack(uint32_t trackID)
{
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        if ( trak->getID() == trackID ) return trak->getTrack();
    }
    error_("getTrack: Can not find track ID: "+std::to_string(trackID));
    return nullptr;
}

void MP4::Parser::error_(std::string message)
{
    std::cout << "Parser: " << filePath_ << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}