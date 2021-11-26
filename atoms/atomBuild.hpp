#ifndef MP4_ATOMBUILD_H
#define MP4_ATOMBUILD_H

#include <memory>
#include "../dataTypes.hpp"

namespace MP4
{

class Processor;

class atomBuild
{
public:
    atomBuild(Processor& processor);

    void        setParentPath(std::string parentPath);
    bool        setNextTrack();
    std::string getParentPath() const;
    uint32_t    getCurrentTrackID() const;
    uint32_t    getVideoTimeScale() const;
    uint32_t    getVideoDuration() const;
    uint32_t    getCreationTime() const;
    uint32_t    getModificationTime() const;
    uint32_t    getNextTrackID() const;
    const std::map<uint32_t, std::shared_ptr<trackType>>&
                getTracks() const;
    std::shared_ptr<trackType>
                getTrack(uint32_t trackID = 0) const;
    uint32_t    getTrackDuration(uint32_t trackID = 0) const;
    uint32_t    getTrackCreationTime(uint32_t trackID = 0) const;
    uint32_t    getTrackModificationTime(uint32_t trackID = 0) const;
    uint32_t    getTrackLayer(uint32_t trackID = 0) const;
    float       getTrackVolume(uint32_t trackID = 0) const;
    float       getTrackWidth(uint32_t trackID = 0) const;
    float       getTrackHeight(uint32_t trackID = 0) const;
    const std::vector<std::vector<float>>&
                getTrackMatrix(uint32_t trackID = 0) const;
    uint32_t    getMediaTimeScale(uint32_t trackID = 0) const;
    uint32_t    getMediaDuration(uint32_t trackID = 0) const;
    uint16_t    getMediaLanguage(uint32_t trackID = 0) const;
    uint16_t    getMediaQuality(uint32_t trackID = 0) const;
    std::string getComponentType(uint32_t trackID = 0) const;
    std::string getComponentSubType(uint32_t trackID = 0) const;
    std::string getComponentName(uint32_t trackID = 0) const;
    uint16_t    getGraphicsMode(uint32_t trackID = 0) const;
    uint16_t    getOpColorR(uint32_t trackID = 0) const;
    uint16_t    getOpColorG(uint32_t trackID = 0) const;
    uint16_t    getOpColorB(uint32_t trackID = 0) const;
    float       getBalance(uint32_t trackID = 0) const;
    std::string getDataFormat(uint32_t trackID = 0) const;
    uint16_t    getDataReferenceIndex(uint32_t trackID = 0) const;
    std::string getDataExtended(uint32_t trackID = 0) const;
    std::map<uint16_t, dataReferenceEntryType>
                getDataReferences(uint32_t trackID = 0) const;
    std::map<uint32_t, editListEntryType>
                getEditList(uint32_t trackID = 0) const;
    std::string getUserData(std::string userDataKey) const;
    bool        samplesHaveSync() const;
    bool        samplesHaveCompositionOffset() const;
    std::vector<uint32_t>
                getReferenceTrackIDs(std::string referenceType, uint32_t trackID = 0) const;
    bool        hasReferenceTrack(uint32_t trackID = 0) const;

private:
    void        error_(std::string message) const;
    uint32_t    newTrackID_(uint32_t trackID) const;

    std::string                                     parentPath_;
    uint32_t                                        currentTrackID_;
    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    std::map<std::string, std::string>              userData_;    
    uint32_t                                        videoTimeScale_;
    uint32_t                                        videoDuration_;
    uint32_t                                        creationTime_;
    uint32_t                                        modificationTime_;
    uint32_t                                        nextTrackID_;
};

}

#endif