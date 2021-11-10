#include "Processor.hpp"
#include <iostream>

MP4::Processor::Processor()
    : videoTimeScale_(0)
    , videoDuration_(0)
    , creationTime_(0)
    , modificationTime_(0)
{
}

uint32_t timeScaleDuration(uint32_t duration, uint32_t sourceTimeScale, uint32_t targetTimeScale)
{
    auto timeScaleMult = (double) sourceTimeScale / targetTimeScale;
    return (uint32_t) (timeScaleMult * (double) duration);
}

void MP4::Processor::addTrack(Parser &parser, uint32_t sourceTrackID, uint32_t targetTrackID)
{
    std::cout << "AddTrack: " << sourceTrackID << " -> " << targetTrackID << std::endl;
    auto parsedTrack = parser.getTrack(sourceTrackID);

    // find tracks with same data format
    // this will change if we can handle multiple tracks of same format
    for ( auto track : tracks_ )
        if ( track.second->dataFormat == parsedTrack->dataFormat )
            error_("addTrack: track of data format '" + parsedTrack->dataFormat + "' already exists");

    uint32_t trackID = 1;
    if ( targetTrackID == 0 )
        // add as next trackID
        while ( tracks_.find(trackID) != tracks_.end() ) trackID++;
    else if ( tracks_.find(targetTrackID) == tracks_.end() )
        // add as target trackID
        trackID = targetTrackID;
    else error_("addTrack: target trackID " + std::to_string(targetTrackID) + " already exists");
    
    //parsedTrack->trackID = trackID;
    tracks_[trackID] = parsedTrack;

    // handle videoTimescale, videoDuration and dates
    // get highest videoTimeScale of 
    videoTimeScale_ = 0;
    for ( auto track : tracks_ )
        if ( track.second->videoTimeScale > videoTimeScale_ )
            videoTimeScale_ = track.second->videoTimeScale;

    // get highest videoDuration based on maxTimeScale
    videoDuration_ = 0;
    for ( auto track : tracks_ ) {
        auto videoDuration = atom::timeScaleDuration( track.second->videoDuration,
            track.second->videoTimeScale, videoTimeScale_);
        if ( videoDuration > videoDuration_ )
            videoDuration_ = videoDuration;
    }

    std::cout << videoTimeScale_ << " " << videoDuration_ << std::endl;

    /*
    for ( auto track : tracks_ ) {
        // get largest track timescale for movie settings
        if ( track.second->mediaTimeScale > timeScale_ ) {
            timeScale_ = track.second->mediaTimeScale;
            duration_ = track.second->mediaDuration;
        }

        // get the newest track times for movie settings
        if ( track.second->creationTime > creationTime_) creationTime_ = track.second->creationTime;
        if ( track.second->modificationTime > modificationTime_) modificationTime_ = track.second->modificationTime;
    }
    */
    // Not correct yet
    /*
    for ( auto track : tracks_ ) {
        // get longest track duration for movie settings (in progress)
        auto videoDuration = atom::timeScaleDuration(track.second->mediaDuration,
            track.second->mediaTimeScale, timeScale_);
        std::cout << "Track: " << track.first << " duration: " << track.second->mediaDuration
            << " -> " << videoDuration << std::endl;
    }
    */

    // set next track after last
    nextTrackID_ = (--tracks_.end())->first + 1;
}

void MP4::Processor::error_(std::string message)
{
    std::cout << "MP4::Processor:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
