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

    // get highest videoTimescale
    videoTimeScale_ = 0;
    for ( auto track : tracks_ )
        if ( track.second->videoTimeScale > videoTimeScale_ )
            videoTimeScale_ = track.second->videoTimeScale;

    // get highest videoDuration based on videoTimeScale_
    videoDuration_ = 0;
    for ( auto track : tracks_ ) {
        auto videoDuration = atom::timeScaleDuration( track.second->videoDuration,
            track.second->videoTimeScale, videoTimeScale_);
        if ( videoDuration > videoDuration_ )
            videoDuration_ = videoDuration;
    }
    
    // set next track after last
    nextTrackID_ = (--tracks_.end())->first + 1;
}

void MP4::Processor::addTrack(Parser &parser, std::string dataFormat, uint32_t targetTrackID)
{
    for ( auto trackID : parser.getDataFormatTrackIDs(dataFormat) ) {
        addTrack(parser, trackID, targetTrackID);
        return;
    }
    error_("addTrack: could not find track with data format: "+dataFormat);
}

void MP4::Processor::addUserData(Parser &parser, std::string userDataKey)
{
    std::map<std::string, std::string> userData;
    parser.getUserData(userData);

    for ( auto entry : userData ) {
        if ( userDataKey == "" || entry.first == userDataKey )
            userData_[entry.first] = entry.second;
    }
}

void MP4::Processor::flattenTrackDurations()
{
    for ( auto track : tracks_ ) {
        auto durationDiff64 =
            (int64_t) track.second->videoDuration - (int64_t) track.second->trackDuration;
        auto lastSampleID = (--track.second->samples.end())->first;
        if ( durationDiff64 == 0 ) continue;
        auto trackDurationDiff = (uint32_t) std::abs(durationDiff64);
        auto mediaDurationDiff = atom::timeScaleDuration(trackDurationDiff,
            track.second->videoTimeScale, track.second->mediaTimeScale);
        if ( track.second->trackDuration < track.second->videoDuration ) {
            track.second->samples[lastSampleID].duration += mediaDurationDiff;
            track.second->samplesDuration += mediaDurationDiff;
            track.second->mediaDuration += mediaDurationDiff;
            track.second->trackDuration += trackDurationDiff;
        } else {
            if ( mediaDurationDiff > track.second->samples[lastSampleID].duration )
                error_("flattenTrackDurations: can not reduce last sample duration");
            track.second->samples[lastSampleID].duration -= mediaDurationDiff;
            track.second->samplesDuration -= mediaDurationDiff;
            track.second->mediaDuration -= mediaDurationDiff;
            track.second->trackDuration -= trackDurationDiff;
        }
    }
}

void MP4::Processor::append(Parser &parser)
{
    flattenTrackDurations();
    for ( auto track : tracks_ ) {
        auto appendTrack = parser.getTrack(track.second->dataFormat);
        if ( appendTrack == nullptr )
            error_("append:  can not find track of data format '"+
                track.second->dataFormat+"' for track: "+std::to_string(track.first));
        if ( appendTrack->mediaTimeScale != track.second->mediaTimeScale )
            error_("append:  different media time scale for track: "+std::to_string(track.first));
        if ( appendTrack->height != track.second->height )
            error_("append:  different height for track: "+std::to_string(track.first));
        if ( appendTrack->width != track.second->width )
            error_("append:  different width for track: "+std::to_string(track.first));

        // append samples
        auto sampleID = (--track.second->samples.end())->first + 1;
        for ( auto sample : appendTrack->samples ) {
            track.second->samples[sampleID] = sample.second;
            track.second->samples[sampleID].time += track.second->mediaDuration;
            sampleID++;
        }
        track.second->samplesDuration += appendTrack->samplesDuration;
        track.second->mediaDuration += appendTrack->mediaDuration;
        track.second->trackDuration += atom::timeScaleDuration(appendTrack->trackDuration,
            appendTrack->videoTimeScale, track.second->videoTimeScale);
        track.second->videoDuration += atom::timeScaleDuration(appendTrack->videoDuration,
            appendTrack->videoTimeScale, track.second->videoTimeScale);
    }

    // get highest videoTimescale
    videoTimeScale_ = 0;
    for ( auto track : tracks_ )
        if ( track.second->videoTimeScale > videoTimeScale_ )
            videoTimeScale_ = track.second->videoTimeScale;

    // get highest videoDuration based on videoTimeScale_
    videoDuration_ = 0;
    for ( auto track : tracks_ ) {
        auto videoDuration = atom::timeScaleDuration( track.second->videoDuration,
            track.second->videoTimeScale, videoTimeScale_);
        if ( videoDuration > videoDuration_ )
            videoDuration_ = videoDuration;
    }
}

void MP4::Processor::test()
{
    std::cout << "\nVideo duration: " << atom::getTimeString(videoDuration_, videoTimeScale_) << std::endl;
    for ( auto track : tracks_ ) {
        auto lastSampleID = (--track.second->samples.end())->first;
        std::cout << "[" << track.first << "]\n"
            << "Video duration: "
            << atom::getTimeString(track.second->videoDuration, track.second->videoTimeScale) << std::endl
            << "Track duration: "
            << atom::getTimeString(track.second->trackDuration, track.second->videoTimeScale) << std::endl
            << "Media duration: "
            << atom::getTimeString(track.second->mediaDuration, track.second->mediaTimeScale) << std::endl
            << "Samples duration: "
            << atom::getTimeString(track.second->samplesDuration, track.second->mediaTimeScale) << std::endl
            << "last sample duration: " << track.second->samples[lastSampleID].duration << std::endl;
    }
}

void MP4::Processor::error_(std::string message)
{
    std::cout << "MP4::Processor:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
