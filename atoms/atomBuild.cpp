#include "atomBuild.hpp"
#include "../Processor.hpp"
#include <iostream>

MP4::atomBuild::atomBuild(Processor &processor)
    : timeScale_(processor.timeScale_)
    , duration_(processor.duration_)
    , creationTime_(processor.creationTime_)
    , modificationTime_(processor.modificationTime_)
    , tracks_(processor.tracks_)
    , currentTrackID_(0)
    , nextTrackID_(processor.nextTrackID_)
{
}

bool MP4::atomBuild::nextTrack()
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

uint32_t MP4::atomBuild::currentTrackID()
{
    return currentTrackID_;
}

uint32_t MP4::atomBuild::getTimeScale()
{
    return timeScale_;
}

uint32_t MP4::atomBuild::getDuration()
{
    return duration_;
}

uint32_t MP4::atomBuild::getCreationTime()
{
    return creationTime_;
}

uint32_t MP4::atomBuild::getModificationTime()
{
    return modificationTime_;
}

uint32_t MP4::atomBuild::getNextTrackID()
{
    return nextTrackID_;
}

uint32_t MP4::atomBuild::getTrackDuration(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->mediaDuration;
}

uint32_t MP4::atomBuild::getTrackCreationTime(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->creationTime;
}

uint32_t MP4::atomBuild::getTrackModificationTime(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->modificationTime;
}

uint32_t MP4::atomBuild::getTrackLayer(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->layer;
}

float MP4::atomBuild::getTrackVolume(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->volume;
}

float MP4::atomBuild::getTrackWidth(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->width;
}

float MP4::atomBuild::getTrackHeight(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->height;
}

std::vector<std::vector<float>> MP4::atomBuild::getTrackMatrix(uint32_t trackID)
{
    return tracks_[newTrackID_(trackID)]->matrix;
}

uint32_t MP4::atomBuild::newTrackID_(uint32_t trackID)
{
    if ( trackID == 0 ) trackID = currentTrackID_;
    if ( tracks_.find(trackID) == tracks_.end() )
        error_("getTrackDuration: trackID does " + std::to_string(trackID) + " does not exist");
    return trackID;
}

void MP4::atomBuild::error_(std::string message)
{
    std::cout << "MP4::atomBuild:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
