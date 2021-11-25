#include "atomBuild.hpp"
#include "../Processor.hpp"
#include <iostream>

MP4::atomBuild::atomBuild(Processor& processor)
    : videoTimeScale_(processor.videoTimeScale_)
    , videoDuration_(processor.videoDuration_)
    , creationTime_(processor.creationTime_)
    , modificationTime_(processor.modificationTime_)
    , tracks_(processor.tracks_)
    , currentTrackID_(0)
    , nextTrackID_(processor.nextTrackID_)
    , userData_(processor.userData_)
{
}

void MP4::atomBuild::setParentPath(std::string parentPath)
{
    parentPath_ = parentPath;
}

bool MP4::atomBuild::setNextTrack()
{
    bool getNext = false;
    if ( currentTrackID_ == 0 ) getNext = true;
    for ( auto track : tracks_ ) {
        if ( getNext ) {
            currentTrackID_ = track.first;
            return true;
        }
        if ( track.first == currentTrackID_ ) getNext = true;
    }
    return false;
}

std::string MP4::atomBuild::getParentPath() const
{
    return parentPath_;
}

uint32_t MP4::atomBuild::getCurrentTrackID() const
{
    return currentTrackID_;
}

uint32_t MP4::atomBuild::getVideoTimeScale() const
{
    return videoTimeScale_;
}

uint32_t MP4::atomBuild::getVideoDuration() const
{
    return videoDuration_;
}

uint32_t MP4::atomBuild::getCreationTime() const
{
    return creationTime_;
}

uint32_t MP4::atomBuild::getModificationTime() const
{
    return modificationTime_;
}

uint32_t MP4::atomBuild::getNextTrackID() const
{
    return nextTrackID_;
}

const std::map<uint32_t, std::shared_ptr<MP4::trackType>>& MP4::atomBuild::getTracks() const
{
    return tracks_;
}

std::shared_ptr<MP4::trackType> MP4::atomBuild::getTrack(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID));
}

uint32_t MP4::atomBuild::getTrackDuration(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->trackDuration;
}

uint32_t MP4::atomBuild::getTrackCreationTime(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->creationTime;
}

uint32_t MP4::atomBuild::getTrackModificationTime(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->modificationTime;
}

uint32_t MP4::atomBuild::getTrackLayer(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->layer;
}

float MP4::atomBuild::getTrackVolume(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->volume;
}

float MP4::atomBuild::getTrackWidth(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->width;
}

float MP4::atomBuild::getTrackHeight(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->height;
}

const std::vector<std::vector<float>>& MP4::atomBuild::getTrackMatrix(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->matrix;
}

uint32_t MP4::atomBuild::getMediaTimeScale(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->mediaTimeScale;
}

uint32_t MP4::atomBuild::getMediaDuration(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->mediaDuration;
}

uint16_t MP4::atomBuild::getMediaLanguage(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->mediaLanguage;
}

uint16_t MP4::atomBuild::getMediaQuality(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->mediaQuality;
}

std::string MP4::atomBuild::getComponentType(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->componentType;
}

std::string MP4::atomBuild::getComponentSubType(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->componentSubType;
}

std::string MP4::atomBuild::getComponentName(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->componentName;
}

uint16_t MP4::atomBuild::getGraphicsMode(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->graphicsMode;
}

uint16_t MP4::atomBuild::getOpColorR(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->opColorR;
}

uint16_t MP4::atomBuild::getOpColorG(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->opColorG;
}

uint16_t MP4::atomBuild::getOpColorB(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->opColorB;
}

std::string MP4::atomBuild::getDataFormat(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->dataFormat;
}

uint16_t MP4::atomBuild::getDataReferenceIndex(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->dataReferenceIndex;
}

std::string MP4::atomBuild::getDataExtended(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->dataExtended;
}

std::map<uint16_t, MP4::dataReferenceEntry> MP4::atomBuild::getDataReferences(uint32_t trackID) const
{
    return tracks_.at(newTrackID_(trackID))->dataReferences;
}

std::string MP4::atomBuild::getUserData(std::string userDataKey) const
{
    std::string userData = "";
    if ( userData_.find(userDataKey) != userData_.end() )
        userData = userData_.at(userDataKey);
    return userData;
}

bool MP4::atomBuild::samplesHaveSync() const
{
    for ( auto sample : tracks_.at(getCurrentTrackID())->samples )
        if ( sample.second.sync ) return true;
    return false;
}

bool MP4::atomBuild::samplesHaveCompositionOffset() const
{
    for ( auto sample : tracks_.at(getCurrentTrackID())->samples )
        if ( sample.second.compositionOffset > 0 ) return true;
    return false;
}

std::vector<uint32_t> MP4::atomBuild::getReferenceTrackIDs(std::string referenceType, uint32_t trackID) const
{
    std::vector<uint32_t> refTrackIDs;
    for ( auto referenceTrackID : tracks_.at(newTrackID_(trackID))->referenceTrackIDs )
        if ( referenceTrackID.second == referenceType ) refTrackIDs.push_back(referenceTrackID.first);
    return refTrackIDs;
}

uint32_t MP4::atomBuild::newTrackID_(uint32_t trackID) const
{
    if ( trackID == 0 ) trackID = currentTrackID_;
    if ( tracks_.find(trackID) == tracks_.end() )
        error_("getTrackDuration: trackID does " + std::to_string(trackID) + " does not exist");
    return trackID;
}

void MP4::atomBuild::error_(std::string message) const
{
    std::cout << "MP4::atomBuild:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
