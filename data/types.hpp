#ifndef MP4_TYPES_H
#define MP4_TYPES_H

#include <stdint.h>
#include <map>
#include <string>

namespace MP4
{

typedef struct sampleType
{
    bool        sync;
    int64_t     filePos;
    uint32_t    size;
    uint32_t    time;
    uint32_t    duration;
} sampleType;

typedef struct trackType
{
    // samples[ID] = sample
    std::map<uint32_t, sampleType>  samples;
    uint32_t                trackID;
    std::string             filePath;
    std::string             dataFormat;
    uint32_t                mediaTimeScale;
    uint32_t                mediaDuration;
    uint32_t                samplesDuration;
} trackType;

}

#endif