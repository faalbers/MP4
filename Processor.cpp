#include "Processor.hpp"
#include <iostream>

MP4::Processor::Processor()
    : timeScale_(0)
    , duration_(0)
{
}

void MP4::Processor::addTrack(Parser &parser, uint32_t trackID)
{
    auto parsedTrack = parser.getTrack(trackID);

    // find tracks with same data format
    for ( auto track : tracks_ )
        if ( track.second->dataFormat == parsedTrack->dataFormat )
            error_("addTrack: track of data format '" + parsedTrack->dataFormat + "' already exists");

    // handle duration
    if ( timeScale_ == 0 ) {
        timeScale_ = parsedTrack->mediaTimeScale;
        duration_ = parsedTrack->mediaDuration;
    }
    
    trackID = 1;
    while ( tracks_.find(trackID) != tracks_.end() ) trackID++;
    parsedTrack->trackID = trackID;
    tracks_[trackID] = parsedTrack;
}

void MP4::Processor::error_(std::string message)
{
    std::cout << "Processor:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
