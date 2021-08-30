#ifndef MP4_DATATYPES_H
#define MP4_DATATYPES_H

#include <stdint.h>

namespace MP4
{

typedef struct sampleType
{
    uint32_t    ID;
    uint32_t    duration;
    uint32_t    time;
    uint32_t    timeOffset;
} sampleType;

typedef struct chunkType
{
    uint32_t    ID;
    uint32_t    samples;
    uint32_t    firstSampleID;
    uint32_t    sampleDescriptionID;
    uint64_t    dataOffset;
} chunkType;

// not sure how to handle this yet
typedef struct chunkOffsetType
{
    uint32_t    ID;
    uint32_t    sampleOffset;
} chunkOffsetType;

}

#endif