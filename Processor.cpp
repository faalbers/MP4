#include "Processor.hpp"
#include <iostream>

MP4::Processor::Processor()
    : videoTimeScale_(0)
    , videoDuration_(0)
    , creationTime_(atom::getCurrentDateTime())
    , modificationTime_(atom::getCurrentDateTime())
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

std::vector<uint32_t> MP4::Processor::addDataFormatTrack(Parser &parser, std::string dataFormat, uint32_t targetTrackID)
{
    for ( auto trackID : parser.getDataFormatTrackIDs(dataFormat) ) {
        return addTrack(parser, trackID, targetTrackID);
    }
    error_("addTrack: could not find track with data format: "+dataFormat);
    std::vector<uint32_t> addedTrackIDs;
    return addedTrackIDs;
}

std::vector<uint32_t> MP4::Processor::addComponentSubTypeTrack(Parser &parser, std::string componentSubType, uint32_t targetTrackID)
{
    for ( auto trackID : parser.getComponentSubTypeTrackIDs(componentSubType) ) {
        return addTrack(parser, trackID, targetTrackID);
    }
    error_("addTrack: could not find track with component subtype: "+componentSubType);
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
        // find difference between track and full video duration
        // move on if there is no difference
        auto durationDiff64 =
            (int64_t) track.second->videoDuration - (int64_t) track.second->trackDuration;
        if ( durationDiff64 == 0 ) continue;

        // get absolute track duration difference and translate it to media duration
        auto trackDurationDiff = (uint32_t) std::abs(durationDiff64);
        auto mediaDurationDiff = atom::timeScaleDuration(trackDurationDiff,
            track.second->videoTimeScale, track.second->mediaTimeScale);

        // find last sample
        auto lastSampleID = (uint32_t) (--track.second->samples.end())->first;
        uint32_t lastEditListID = 0;
        if ( track.second->editList.size() > 0 )
            lastEditListID = (uint32_t) (--track.second->editList.end())->first;
        if ( track.second->trackDuration < track.second->videoDuration ) {
            std::cout << "add duration\n";
            // add media duration difference on last sample
            track.second->samples[lastSampleID].duration += mediaDurationDiff;
            // apply to totals
            track.second->samplesDuration += mediaDurationDiff;
            track.second->mediaDuration += mediaDurationDiff;
            // apply to track duration
            track.second->trackDuration += trackDurationDiff;
            // apply to elst duration
            if ( lastEditListID != 0 )
                track.second->editList[lastEditListID].trackDuration += mediaDurationDiff;
        } else {
            std::cout << "reduce duration\n";
            // reduce media duration difference on last sample
            // make sure reduction is not greater then last sample duration
            if ( mediaDurationDiff > track.second->samples[lastSampleID].duration )
                error_("flattenTrackDurations: can not reduce last sample duration");
            track.second->samples[lastSampleID].duration -= mediaDurationDiff;
            // apply to totals
            track.second->samplesDuration -= mediaDurationDiff;
            track.second->mediaDuration -= mediaDurationDiff;
            // apply to track duration
            track.second->trackDuration -= trackDurationDiff;
            // apply to elst duration
            if ( lastEditListID != 0 ) {
                if ( mediaDurationDiff > track.second->editList[lastEditListID].trackDuration )
                    error_("flattenTrackDurations: can not reduce last edit list duration");
                track.second->editList[lastEditListID].trackDuration -= mediaDurationDiff;
            }
        }
    }
}

void MP4::Processor::append(Parser &parser)
{
    flattenTrackDurations();
    for ( auto track : tracks_ ) {
        // find track with same data format, timescale and size
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
        
        // append edit list
        editListEntryType editListEntry;
        if ( track.second->editList.size() > 0 ) {
            auto lastEditID = (--track.second->editList.end())->first;
            auto lastMediaRate = track.second->editList[lastEditID].mediaRate;
            auto lastTrackDuration = track.second->editList[lastEditID].trackDuration;
            auto lastMediaStartTime = track.second->editList[lastEditID].mediaStartTime;
            for ( auto edit : appendTrack->editList ) {
                if ( edit.second.mediaRate == lastMediaRate ) {
                    track.second->editList[lastEditID].trackDuration += edit.second.trackDuration;
                } else {
                    editListEntry.mediaRate = edit.second.mediaRate;
                    editListEntry.trackDuration = edit.second.trackDuration;
                    editListEntry.mediaStartTime = lastMediaStartTime + lastTrackDuration + edit.second.mediaStartTime;
                    lastEditID++;
                    lastMediaRate = editListEntry.mediaRate;
                    lastTrackDuration = edit.second.trackDuration;
                    lastMediaStartTime = editListEntry.mediaStartTime;
                    track.second->editList[lastEditID] = editListEntry;
                }
            }
        }
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

void MP4::Processor::info()
{
    std::cout << "\nCurrent Processor Information:";
    std::cout << "\n------------------------------\n";
    std::cout << "\tvideo duration   : " << videoDuration_ << " ("
        << atom::getTimeString(videoDuration_, videoTimeScale_) << ")" << std::endl;
    std::cout << "\tcreation time    : " << atom::getDateTimeString(creationTime_) << std::endl;
    std::cout << "Track Information:\n";
    std::cout << "------------------\n";
    for ( auto track : tracks_) {
        std::cout << "Track: " << track.first;
        if ( track.second->enforcedTrackID ) std::cout << " (enforced track ID)";
        std::cout << std::endl;
        std::cout << "\tcomponentSubType  : " << track.second->componentSubType << std::endl;
        std::cout << "\tdataFormat        : " << track.second->dataFormat << std::endl;
        std::cout << "\tdataReferenceIndex: " << track.second->dataReferenceIndex << std::endl;
        std::cout << "\tmedia duration    : " << track.second->mediaDuration << " ("
            << atom::getTimeString(track.second->mediaDuration, track.second->mediaTimeScale)
            << ")" << std::endl;
        if ( track.second->componentSubType == "vide") {
            std::cout << "\tvideo width       : " << track.second->width << std::endl;
            std::cout << "\tvideo height      : " << track.second->height << std::endl;
        }
        if ( track.second->componentSubType == "soun") {
            std::cout << "\tsound balance     : " << track.second->balance << std::endl;
        }
        std::cout << "\tcreation time     : " << atom::getDateTimeString(track.second->creationTime) << std::endl;
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
        if ( track.second->dataReferences.size() > 0 ) {
            std::cout << "\tdata references:\n";
            std::cout << "\t\t[#] ( type , dataInSameFile )\n";
            for ( auto dataReference : track.second->dataReferences ) {
                std::cout << "\t\t[" << dataReference.first << "]"
                    << " ( " << dataReference.second.type
                    << ", " << dataReference.second.dataInSameFile
                    << " )" << std::endl;
            }
        }
        if ( track.second->editList.size() > 0 ) {
            std::cout << "\tedit list:\n";
            std::cout << "\t\t[#] ( trackDuration , mediaStartTime, mediaRate )\n";
            for ( auto entry : track.second->editList ) {
                std::cout << "\t\t[" << entry.first << "]"
                    << " ( " << entry.second.trackDuration
                    << " (" << atom::getTimeString(entry.second.trackDuration,  videoTimeScale_)
                    << "), " << entry.second.mediaStartTime
                    << " (" << atom::getTimeString(entry.second.mediaStartTime, videoTimeScale_)
                    << "), " << entry.second.mediaRate
                    << " )" << std::endl;
            }
        }
    }
    if ( userData_.size() > 0 ) {
        std::cout << "User Data Information:\n";
        std::cout << "----------------------\n";
        for ( auto userData : userData_ ) std::cout << "\t- " << userData.first << std::endl;
    }
}

void MP4::Processor::error_(std::string message) const
{
    std::cout << "MP4::Processor:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
