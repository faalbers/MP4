#ifndef MP4_ATOMBUILD_H
#define MP4_ATOMBUILD_H

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace MP4
{

typedef struct sampleType
{
    std::string filePath;
    int64_t     filePos;
    uint32_t    size;
    uint32_t    time;
    uint32_t    duration;
    bool        sync;
    uint32_t    chunkID;
    uint32_t    compositionOffset;
} sampleType;

typedef struct chunkType
{
    uint32_t    samples;
    uint32_t    firstSampleID;
    uint32_t    sampleDescriptionID;
    uint64_t    dataOffset;
} chunkType;

typedef struct trackType
{
    // samples[ID] = sample
    std::map<uint32_t, sampleType>  samples;
    std::map<uint32_t, chunkType>   createdChunks;
    uint32_t                creationTime;
    uint32_t                modificationTime;
    uint32_t                trackID;
    std::string             dataFormat;
    uint16_t                dataReferenceIndex;
    std::string             dataExtended;
    uint32_t                videoTimeScale;
    uint32_t                videoDuration;
    uint32_t                trackDuration;
    uint32_t                mediaTimeScale;
    uint32_t                mediaDuration;
    uint16_t                layer;
    uint32_t                samplesDuration;
    float                   volume;
    float                   width;
    float                   height;
    std::vector<std::vector<float>> matrix;
    uint16_t                mediaLanguage;
    uint16_t                mediaQuality;
    std::string             componentType;
    std::string             componentSubType;
    std::string             componentName;
    uint16_t                graphicsMode;
    uint16_t                opColorR;
    uint16_t                opColorG;
    uint16_t                opColorB;
} trackType;

class Processor;

class atomBuild
{
public:
    atomBuild(Processor& processor);

    std::string parentPath;
    bool        nextTrack();
    uint32_t    currentTrackID() const;
    uint32_t    getVideoTimeScale() const;
    uint32_t    getVideoDuration() const;
    uint32_t    getCreationTime() const;
    uint32_t    getModificationTime() const;
    uint32_t    getNextTrackID() const;
    std::map<uint32_t, std::shared_ptr<trackType>>
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
    std::vector<std::vector<float>>
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
    std::string getDataFormat(uint32_t trackID = 0) const;
    uint16_t    getDataReferenceIndex(uint32_t trackID = 0) const;
    std::string getDataExtended(uint32_t trackID = 0) const;
    std::string getUserData(std::string userDataKey) const;
    bool        samplesHaveSync() const;
    bool        samplesHaveCompositionOffset() const;

private:
    void        error_(std::string message) const;
    uint32_t    newTrackID_(uint32_t trackID) const;

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