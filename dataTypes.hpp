#ifndef MP4_DATATYPES_H
#define MP4_DATATYPES_H

#include <string>
#include <map>
#include <vector>

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

typedef struct dataReferenceEntry
{
    std::string type;
    bool        dataInSameFile;
} dataReferenceEntry;

typedef struct trackType
{
    // samples[ID] = sample
    std::map<uint32_t, sampleType>  samples;
    std::map<uint32_t, chunkType>   createdChunks;
    uint32_t                creationTime;
    uint32_t                modificationTime;
    bool                    enforcedTrackID;
    std::map<std::string, uint32_t>
                            sourceTrackIDs;
    std::map<std::string, std::map<uint32_t, std::string>>
                            sourceRefTrackIDs;
    std::map<uint32_t, std::string>
                            referenceTrackIDs;
    std::string             dataFormat;
    uint16_t                dataReferenceIndex;
    std::map<uint16_t, dataReferenceEntry>
                            dataReferences;
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
    std::vector<std::vector<float>>
                            matrix;
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

}

#endif