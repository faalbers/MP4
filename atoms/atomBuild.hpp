#ifndef MP4_ATOMBUILD_H
#define MP4_ATOMBUILD_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../types.hpp"

namespace MP4
{

class Processor;

class atomBuild
{
public:
    atomBuild(Processor &processor);

    std::string parentPath;
    bool        nextTrack();
    uint32_t    currentTrackID();
    uint32_t    getTimeScale();
    uint32_t    getDuration();
    uint32_t    getCreationTime();
    uint32_t    getModificationTime();
    uint32_t    getNextTrackID();
    uint32_t    getTrackDuration(uint32_t trackID = 0);
    uint32_t    getTrackCreationTime(uint32_t trackID = 0);
    uint32_t    getTrackModificationTime(uint32_t trackID = 0);
    uint32_t    getTrackLayer(uint32_t trackID = 0);
    float       getTrackVolume(uint32_t trackID = 0);
    float       getTrackWidth(uint32_t trackID = 0);
    float       getTrackHeight(uint32_t trackID = 0);
    std::vector<std::vector<float>> getTrackMatrix(uint32_t trackID = 0);

private:
    void        error_(std::string message);
    uint32_t    newTrackID_(uint32_t trackID);

    uint32_t                                        currentTrackID_;
    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    uint32_t                                        timeScale_;
    uint32_t                                        duration_;
    uint32_t                                        creationTime_;
    uint32_t                                        modificationTime_;
    uint32_t                                        nextTrackID_;
};

}

#endif