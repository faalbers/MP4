#include "Processor.hpp"
#include <iostream>

MP4::Processor::Processor()
    : videoTimeScale_(0)
    , videoDuration_(0)
    , creationTime_(0)
    , modificationTime_(0)
{
}

uint32_t MP4::Processor::getNextAvailableTrackID()
{
    uint32_t nextTrackID = 1;
    while (tracks_.find(nextTrackID) != tracks_.end()) nextTrackID++;
    return nextTrackID;
}

bool MP4::Processor::moveUpTrackID(uint32_t trackID)
{
    if (tracks_.find(trackID) == tracks_.end()) return true;
    else if (tracks_[trackID]->enforcedTrackID) return false;
    
    auto nextTrackID = getNextAvailableTrackID();
    tracks_[nextTrackID] = tracks_[trackID];
    tracks_.erase(trackID);

    // update track references
    for ( auto track : tracks_ ) {
        if ( track.second->referenceTrackIDs.find(trackID) != track.second->referenceTrackIDs.end() ) {
            track.second->referenceTrackIDs[nextTrackID] = track.second->referenceTrackIDs[trackID];
            track.second->referenceTrackIDs.erase(trackID);
        }
    }

    return true;
}

std::vector<uint32_t> MP4::Processor::addTrack(Parser &parser, uint32_t sourceTrackID, uint32_t targetTrackID)
{
    std::vector<uint32_t> addedTrackIDs;
    auto parsedTrack = parser.getTrack(sourceTrackID);
    auto parsedFilePath = parser.getFilePath();

    // find tracks with same data format
    // this will change if we can handle multiple tracks of same format
    for ( auto track : tracks_ )
        if ( track.second->dataFormat == parsedTrack->dataFormat )
            error_("addTrack: track of data format '" + parsedTrack->dataFormat + "' already exists");

    uint32_t trackID;
    if ( targetTrackID == 0 ) trackID = getNextAvailableTrackID();
    else if ( moveUpTrackID(targetTrackID) ) trackID = targetTrackID;
    else error_("addTrack: target trackID " + std::to_string(targetTrackID) + " already exists");
    tracks_[trackID] = parsedTrack;
    addedTrackIDs.push_back(trackID);
    if ( targetTrackID != 0 ) tracks_[trackID]->enforcedTrackID = true;

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

    // check if we need to add a referenced track
    if (tracks_[trackID]->sourceRefTrackIDs.find(parsedFilePath) != tracks_[trackID]->sourceRefTrackIDs.end()) {
        for ( auto sourceRefTrackID : tracks_[trackID]->sourceRefTrackIDs[parsedFilePath] ) {
            for ( auto track : tracks_ ) {
                if ( track.first == trackID ) continue;
                for ( auto sourceTrackID : track.second->sourceTrackIDs ) {
                    if (sourceTrackID.first == parsedFilePath && sourceTrackID.second == sourceRefTrackID.first) {
                        // referenced track already in current tracks
                        tracks_[trackID]->referenceTrackIDs[track.first] = sourceRefTrackID.second;
                        return addedTrackIDs;
                    }
                }
            }
            // adding referenced track
            auto refTrackIDs = addTrack(parser, sourceRefTrackID.first);
            tracks_[trackID]->referenceTrackIDs[refTrackIDs[0]] = sourceRefTrackID.second;
            addedTrackIDs.insert(addedTrackIDs.end(), refTrackIDs.begin(), refTrackIDs.end());
            return addedTrackIDs;
        }
    }
    return addedTrackIDs;
}

std::vector<uint32_t> MP4::Processor::addTrack(Parser &parser, std::string dataFormat, uint32_t targetTrackID)
{
    for ( auto trackID : parser.getDataFormatTrackIDs(dataFormat) ) {
        return addTrack(parser, trackID, targetTrackID);
    }
    error_("addTrack: could not find track with data format: "+dataFormat);
    std::vector<uint32_t> addedTrackIDs;
    return addedTrackIDs;
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

void MP4::Processor::info()
{
    for ( auto track : tracks_) {
        std::cout << "Track: " << track.first;
        if ( track.second->enforcedTrackID ) std::cout << " (enforced track ID)";
        std::cout << std::endl;
        std::cout << "\tcomponentSubType: " << track.second->componentSubType << std::endl;
        std::cout << "\tdataFormat      : " << track.second->dataFormat << std::endl;
        if ( track.second->sourceTrackIDs.size() > 0 ) {
            std::cout << "\tsource tracks:\n";
            for ( auto sourceTrackID : track.second->sourceTrackIDs )
                std::cout << "\t\ttrack " << sourceTrackID.second
                    << " in file " << sourceTrackID.first << std::endl;
        }
        if ( track.second->referenceTrackIDs.size() > 0 ) {
            std::cout << "\treferenced tracks:\n";
            for ( auto referenceTrackID : track.second->referenceTrackIDs )
                std::cout << "\t\ttrack " << referenceTrackID.first
                    << " of type '" << referenceTrackID.second << "'\n";
        }
        if ( track.second->sourceRefTrackIDs.size() > 0 ) {
            std::cout << "\treferenced source tracks:\n";
            for ( auto sourceRefTrackIDs : track.second->sourceRefTrackIDs ) {
                for ( auto sourceRefTrackID : sourceRefTrackIDs.second )
                    std::cout << "\t\ttrack " << sourceRefTrackID.first
                        << " of type '" << sourceRefTrackID.second
                        << "' in file " << sourceRefTrackIDs.first << std::endl;
            }
        }
    }
}

void MP4::Processor::error_(std::string message) const
{
    std::cout << "MP4::Processor:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
