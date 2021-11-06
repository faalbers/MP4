#include "Processor.hpp"
#include <iostream>

MP4::Processor::Processor()
    : timeScale_(0)
    , duration_(0)
    , creationTime_(0)
    , modificationTime_(0)
{
}

void MP4::Processor::addTrack(Parser &parser, uint32_t sourceTrackID, uint32_t targetTrackID)
{
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
    
    parsedTrack->trackID = trackID;
    tracks_[trackID] = parsedTrack;

    // handle timescale, duration and dates
    for ( auto track : tracks_ ) {
        if ( track.second->mediaTimeScale > timeScale_ ) {
            timeScale_ = track.second->mediaTimeScale;
            std::cout << "TimeScale changed: " << timeScale_ << std::endl;
            duration_ = track.second->mediaDuration;
            std::cout << "Duration changed: " << duration_ << std::endl;
        }
        if ( track.second->creationTime > creationTime_) creationTime_ = track.second->creationTime;
        if ( track.second->modificationTime > modificationTime_) modificationTime_ = track.second->modificationTime;
    }

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
