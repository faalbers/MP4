#include "MP4.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <algorithm>
#include "atoms_create/root_create.hpp"
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

void MP4::MP4::splunkCreate(splunkType &splunk)
{
    splunk.fileWritePath = std::filesystem::absolute(std::filesystem::path(splunk.fileWritePath)).string();

    std::ofstream fileWrite(splunk.fileWritePath, std::ios::binary);
    if ( fileWrite.fail() ) throw std::runtime_error("Can not write MP4 file: "+splunk.fileWritePath);
    splunk.fileWrite = &fileWrite;

    rootAtom_->create(splunk);

    fileWrite.close();
    splunk.fileWrite = nullptr;
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

MP4::splunkType MP4::MP4::splunkGetB()
{
    splunkType splunk;

    // initializing unused data
    splunk.fileWrite = nullptr;
    splunk.fileWritePath = "";
    splunk.rootAtom = rootAtom_.get();

    // get video time scale and duration
    for ( auto mvhd : getTypeAtoms<mvhd>() ) {
        splunk.videoTimeScale = mvhd->timeScale;
        splunk.videoDuration = mvhd->duration;
    }

    std::vector<trackSamplesType> tracksSamples;
    for ( auto track : getTracks() ) {
        auto samples = track->getSamples();

        // exclude tracks where sample duration does not match like fdsc
        if ( samples.mediaDuration != samples.samplesDuration ) continue;

        // add to tracks
        trackInfoType trackInfo;
        trackInfo.dataFormat = samples.dataFormat;
        trackInfo.sampleCount = samples.sampleCount;
        splunk.tracks[samples.trackID] = trackInfo;

        // flatten media duration to video duration
        auto timeScaleMult = (double) splunk.videoTimeScale / samples.mediaTimeScale;
        double toVideoTimeScale = timeScaleMult * (double) samples.mediaDuration;
        int64_t timeDifference = ((double) splunk.videoDuration - toVideoTimeScale) / timeScaleMult;
        
        // get the track samples and flatten last sample duration with difference to video duration
        auto trackSamples = samples;
        auto updatedDuration = std::max((int64_t)trackSamples.samples.back().duration + timeDifference, (int64_t)0);
        trackSamples.samples.back().duration = (uint32_t)updatedDuration;
        
        // reverse for popping
        std::reverse(trackSamples.samples.begin(),trackSamples.samples.end());

        tracksSamples.push_back(trackSamples);
    }

    bool samplesDepleted = false;
    do {
        std::map<int64_t, trackSamplesType *> positionMap;
        for ( int trackIndex = 0; trackIndex < tracksSamples.size(); trackIndex++ ) {
            if ( tracksSamples[trackIndex].samples.size() != 0 ) {
                auto filePos = tracksSamples[trackIndex].samples.back().filePos;
                positionMap[filePos] = &tracksSamples[trackIndex];
            }
        }
        if ( positionMap.size() != 0 ) {
            for ( auto samples : positionMap ) {
                splunkSampleType splunkSample;
                splunkSample.ID = samples.second->samples.back().ID;
                splunkSample.filePos = samples.second->samples.back().filePos;
                splunkSample.size = samples.second->samples.back().size;
                splunkSample.time = samples.second->samples.back().time;
                splunkSample.duration = samples.second->samples.back().duration;
                splunkSample.trackID = samples.second->trackID;
                splunkSample.filePath = samples.second->filePath;
                splunkSample.timeScale = samples.second->mediaTimeScale;
                splunkSample.format = samples.second->dataFormat;
                splunk.samples.push_back(splunkSample);
                samples.second->samples.pop_back();
                break;
            }
        } else samplesDepleted = true;
    } while ( !samplesDepleted );

    return splunk;
}

MP4::splunkType MP4::MP4::splunkGet()
{
    splunkType splunk;

    // initializing unused data
    splunk.fileWrite = nullptr;
    splunk.fileWritePath = "";
    splunk.rootAtom = rootAtom_.get();

    // get video time scale and duration
    for ( auto mvhd : getTypeAtoms<mvhd>() ) {
        splunk.videoTimeScale = mvhd->timeScale;
        splunk.videoDuration = mvhd->duration;
    }

    std::vector<trackSamplesType> tracksSamples;
    for ( auto track : getTracks() ) {
        auto samples = track->getSamples();

        // exclude tracks where sample duration does not match like fdsc
        if ( samples.mediaDuration != samples.samplesDuration ) continue;

        // add to tracks
        trackInfoType trackInfo;
        trackInfo.dataFormat = samples.dataFormat;
        trackInfo.sampleCount = samples.sampleCount;
        splunk.tracks[samples.trackID] = trackInfo;

        // flatten media duration to video duration
        auto timeScaleMult = (double) splunk.videoTimeScale / samples.mediaTimeScale;
        double toVideoTimeScale = timeScaleMult * (double) samples.mediaDuration;
        int64_t timeDifference = ((double) splunk.videoDuration - toVideoTimeScale) / timeScaleMult;
        
        // get the track samples and flatten last sample duration with difference to video duration
        auto trackSamples = samples;
        auto updatedDuration = std::max((int64_t)trackSamples.samples.back().duration + timeDifference, (int64_t)0);
        trackSamples.samples.back().duration = (uint32_t)updatedDuration;
        
        // reverse for popping
        std::reverse(trackSamples.samples.begin(),trackSamples.samples.end());

        tracksSamples.push_back(trackSamples);
    }

    uint32_t time = 0;
    bool samplesDepleted = false;
    do {
        std::map<uint32_t, trackSamplesType *> timeMap;
        for ( int trackIndex = 0; trackIndex < tracksSamples.size(); trackIndex++ ) {
            if ( tracksSamples[trackIndex].samples.size() != 0 ) {
                auto timeScaleMult = (double) splunk.videoTimeScale / tracksSamples[trackIndex].mediaTimeScale;
                auto toVideoTimeScale = (uint32_t) (timeScaleMult * tracksSamples[trackIndex].samples.back().time);
                timeMap[toVideoTimeScale] = &tracksSamples[trackIndex];
            }
        }
        if ( timeMap.size() != 0 ) {
            for ( auto samples : timeMap ) {
                splunkSampleType splunkSample;
                splunkSample.ID = samples.second->samples.back().ID;
                splunkSample.filePos = samples.second->samples.back().filePos;
                splunkSample.size = samples.second->samples.back().size;
                splunkSample.time = samples.second->samples.back().time;
                splunkSample.duration = samples.second->samples.back().duration;
                splunkSample.trackID = samples.second->trackID;
                splunkSample.filePath = samples.second->filePath;
                splunkSample.timeScale = samples.second->mediaTimeScale;
                splunkSample.format = samples.second->dataFormat;
                splunk.samples.push_back(splunkSample);
                time = samples.first;
                samples.second->samples.pop_back();
                break;
            }
        } else samplesDepleted = true;
    } while ( !samplesDepleted );

    return splunk;
}

MP4::splunkType MP4::MP4::splunkAppend(MP4 &appendMP4)
{
    auto splunk = splunkGet();

    std::set<std::string> formats;
    for ( auto track : splunk.tracks ) {
        if ( formats.find(track.second.dataFormat) != formats.end() )
            throw std::runtime_error("MP4::MP4::splunkAppend multiple tracks with same format not handled yet");
        formats.insert(track.second.dataFormat);
    }
    auto splunkAppend = appendMP4.splunkGet();
    formats.clear();
    for ( auto track : splunkAppend.tracks ) {
        if ( formats.find(track.second.dataFormat) != formats.end() )
            throw std::runtime_error("MP4::MP4::splunkAppend multiple tracks with same format not handled yet");
        formats.insert(track.second.dataFormat);
    }

    // trackMatch[appendTrackID] = currentTrackID
    std::map<uint32_t, uint32_t> trackMatch;
    for ( auto appendTrack: splunkAppend.tracks ) {
        for ( auto currentTrack : splunk.tracks ) {
            if ( appendTrack.second.dataFormat == currentTrack.second.dataFormat ) {
                trackMatch[appendTrack.first] = currentTrack.first;
                break;
            }
        }
    }

    // add samples if in track match
    auto tracks = splunk.tracks;
    for ( auto appendSample : splunkAppend.samples ) {
        appendSample.trackID = trackMatch[appendSample.trackID];
        appendSample.ID += tracks[appendSample.trackID].sampleCount;
        splunk.tracks[appendSample.trackID].sampleCount = appendSample.ID;
        splunk.samples.push_back(appendSample);
    }

    // update total video duration
    splunk.videoDuration += splunkAppend.videoDuration;

    return splunk;
}
