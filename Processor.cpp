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
    if ( parsedTrack == nullptr ) return;

    // find tracks with same data format
    for ( auto track : tracks_ ) {
        if ( track.second->dataFormat == parsedTrack->dataFormat ) {

        }
    }
}
