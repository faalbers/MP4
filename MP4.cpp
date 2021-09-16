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

    internal::atomBuildType atomBuild;
    atomBuild.filePath = filePath;
    rootAtom_ = std::make_shared<root>(atomBuild);
    
    #ifdef MP4_PARSE_TIME
    auto end = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nMP4 Time : " << ms_int.count() << "ms\n";
    #endif
}

std::vector<MP4::trak *>  MP4::MP4::getTracks()
{
    for ( auto moov : rootAtom_->getTypeAtoms<moov>()) return moov->getTypeAtoms<trak>();
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
    return rootAtom_->nestLevel_(0);
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

    rootAtom_->printHierarchy(pathWith, valWith);
    
    std::cout << std::endl;
}

void MP4::MP4::printHierarchyData(bool fullLists)
{
    //std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    std::cout << std::endl;
    rootAtom_->printHierarchyData(fullLists);
}

void MP4::MP4::write(std::string filePath_, writeSettingsType &writeSettings)
{
    std::ofstream fileWrite(filePath_, std::ios::binary);
    if ( fileWrite.fail() ) throw std::runtime_error("Can not write MP4 file: "+filePath_);

    // data to pass through write process
    internal::writeInfoType writeInfo;

    // exclude tracks with incorrect timing
    for ( auto track : getTracks() )
        for ( auto mdhd : track->getTypeAtoms<mdhd>() )
            for ( auto stts : track->getTypeAtoms<stts>() ) {
                uint32_t totalDuration = 0;
                for ( auto entry : stts->sttsTable )
                    totalDuration += entry[0] * entry[1];
                if ( mdhd->duration != totalDuration )
                    writeSettings.excludeTrackIDs.insert(track->getID());
            }

    // make final track include map
    for ( auto track : getTracks() ) {
        std::set<uint32_t>::iterator it = writeSettings.excludeTrackIDs.find(track->getID());
        if( it != writeSettings.excludeTrackIDs.end() ) continue;
        writeInfo.includeTrackIDs[track->getID()] = track->getID();
    }

    // first write ftyp
    for ( auto child : getTypeAtoms<ftyp>() ) child->write(fileWrite, writeInfo);

    // write mdat 
    for ( auto child : getTypeAtoms<mdat>() ) child->write(fileWrite, writeInfo);
    
    // write moov 
    for ( auto moov : getTypeAtoms<moov>() ) moov->write(fileWrite, writeInfo);

    fileWrite.close();
}

void MP4::MP4::append(MP4 &appendMP4, std::string filePath_, writeSettingsType &writeSettings)
{
    std::ofstream fileWrite(filePath_, std::ios::binary);
    if ( fileWrite.fail() ) throw std::runtime_error("Can not write MP4 file: "+filePath_);

    // data to pass through write process
    internal::writeInfoType writeInfo;

    // exclude tracks with incorrect timing
    for ( auto track : getTracks() )
        for ( auto mdhd : track->getTypeAtoms<mdhd>() )
            for ( auto stts : track->getTypeAtoms<stts>() ) {
                uint32_t totalDuration = 0;
                for ( auto entry : stts->sttsTable )
                    totalDuration += entry[0] * entry[1];
                if ( mdhd->duration != totalDuration )
                    writeSettings.excludeTrackIDs.insert(track->getID());
            }

    // make final track include map
    for ( auto mainTrack : getTracks() ) {
        std::set<uint32_t>::iterator it = writeSettings.excludeTrackIDs.find(mainTrack->getID());
        if( it != writeSettings.excludeTrackIDs.end() ) continue;
        for ( auto mainStsd : mainTrack->getTypeAtoms<stsd>() ) {
            for ( auto appendTrack : appendMP4.getTracks() ) {
                for ( auto appendStsd : appendTrack->getTypeAtoms<stsd>() ) {
                    for ( auto mainEntry : mainStsd->stsdTable ) {
                        for ( auto appendEntry : appendStsd->stsdTable ) {
                            if ( mainEntry.dataFormat == appendEntry.dataFormat ) {
                                writeInfo.includeTrackIDs[mainTrack->getID()] = appendTrack->getID();
                            }
                        }
                    }
                }
            }
        }
    }

    // first write ftyp
    for ( auto child : getTypeAtoms<ftyp>() ) child->write(fileWrite, writeInfo);

    // append mdat
    for ( auto mdatMaster : getTypeAtoms<mdat>() ) {
        for ( auto mdatAppend : appendMP4.getTypeAtoms<mdat>() )
            mdatMaster->append(mdatAppend, fileWrite, writeInfo);
    }

    // append moov
    for ( auto moovMaster : getTypeAtoms<moov>() ) {
        for ( auto moovAppend : appendMP4.getTypeAtoms<moov>() )
            moovMaster->append(moovAppend, fileWrite, writeInfo);
    }

    fileWrite.close();
}

MP4::mdatCreateType MP4::MP4::getMdatCreate()
{
    mdatCreateType mdatCreate;
/*
    for ( auto track : getTracks() ) {
        auto samples = track->getSamplesNew();
        if ( samples.mediaDuration != samples.samplesDuration ) continue;
        if ( samples.trackID == 2 ) {
            for ( auto sample : samples.samples ) {
                std::cout
                    << "[" << sample.trackID << "]"
                    << "[" << sample.ID << "] "
                    << sample.time
                    << " " << sample.duration 
                    << " " << sample.filePos 
                    << " " << sample.size << " " << std::endl;
            }
            break;
        }
    }
    return mdatCreate;
*/
    // get video time scale and duration
    for ( auto mdhd : getTypeAtoms<mdhd>() ) {
        mdatCreate.timeScale = mdhd->timeScale;
        mdatCreate.duration = mdhd->duration;
    }

    std::vector<std::vector<sampleNewType>> tracksSamples;
    for ( auto track : getTracks() ) {
        auto samples = track->getSamplesNew();
        if ( samples.mediaDuration != samples.samplesDuration ) continue;

        /*
        std::cout << "Track ID          : " << samples.trackID << std::endl;
        std::cout << "Sample Count      : " << samples.sampleCount << std::endl;
        std::cout << "File Path         : " << samples.filePath << std::endl;
        std::cout << "Data Format       : " << samples.dataFormat << std::endl;
        std::cout << "Media Time Scale  : " << samples.mediaTimeScale << std::endl;
        std::cout << "Media Time Scale  : " << samples.mediaDuration << std::endl;
        std::cout << "Samples Time Scale: " << samples.samplesDuration << std::endl << std::endl;
        */

        // get the track samples and reverse the vector so we can pop
        auto trackSamples = samples.samples;
        std::reverse(trackSamples.begin(),trackSamples.end());

        // convert time and durations to video time scale
        auto timeScaleMult = (double) mdatCreate.timeScale / samples.mediaTimeScale;
        for ( size_t index = 0; index < trackSamples.size(); index++ ) {
            trackSamples[index].time = (uint32_t) (timeScaleMult * trackSamples[index].time);
            trackSamples[index].duration = (uint32_t) (timeScaleMult * trackSamples[index].duration);
        }

        tracksSamples.push_back(trackSamples);
    }

    uint32_t time = 0;
    bool samplesDepleted = false;
    do {
        std::map<uint32_t, std::vector<sampleNewType> *> timeMap;
        for ( int trackIndex = 0; trackIndex < tracksSamples.size(); trackIndex++ ) {
            if ( tracksSamples[trackIndex].size() != 0 )
                timeMap[tracksSamples[trackIndex].back().time] = &tracksSamples[trackIndex];
        }
        if ( timeMap.size() != 0 ) {
            for ( auto samples : timeMap ) {
                mdatCreate.samples.push_back(samples.second->back());
                time = samples.first;
                samples.second->pop_back();
                break;
            }
        } else samplesDepleted = true;
    } while ( !samplesDepleted );

    return mdatCreate;
}