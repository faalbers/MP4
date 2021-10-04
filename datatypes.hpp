#ifndef MP4_DATATYPES_H
#define MP4_DATATYPES_H

#include <stdint.h>
#include <string>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <fstream>

namespace MP4
{

// ********* DATA TYPES *********
// ID's in MP4 always start with 1 , not 0
class root;
class moov;
class trak;
class MP4;

typedef struct sampleType
{
    bool        sync;
    int64_t     filePos;
    uint32_t    size;
    uint32_t    time;
    uint32_t    duration;
} sampleType;

typedef struct samplesType
{
    std::map<uint32_t, sampleType>  samples;
    uint32_t                trackID;
    std::string             filePath;
    std::string             dataFormat;
    uint32_t                mediaTimeScale;
    uint32_t                mediaDuration;
    uint32_t                samplesDuration;
} samplesType;

typedef struct chunkType
{
    uint32_t    samples;
    uint32_t    firstSampleID;
    uint32_t    currentSampleID;
    uint32_t    sampleDescriptionID;
    uint64_t    dataOffset;
} chunkType;

typedef struct writeSettingsType
{
    std::set<uint32_t>  excludeTrackIDs;
} writeSettingsType;

typedef struct appendSettingsType
{
    std::set<uint32_t>  excludeTrackIDs;
} appendSettingsType;

typedef struct stsdEntryType
{
    std::string dataFormat;         // format type FourCC
    uint16_t    dataReferenceIndex; // index of the data reference to use to retrieve data associated
                                    // with samples that use this sample description.
                                    // Data references are stored in data reference atoms
    std::string extendedData;
} stsdEntryType;

typedef struct elstEntryType
{
    uint32_t    duration;
    uint32_t    mediaTime;
    float       mediaRate;
} elstEntryType;

namespace internal
{
// ********* INTERNAL DATA *********
// remember they are 32 bit aligned !

typedef struct atomBuildType
{
    std::string filePath;
    int64_t     filePos;
    std::string parentPath;
} atomBuildType;


typedef struct writeInfoType
{
    std::map<uint32_t, uint32_t>                            includeTrackIDs;
    std::vector<std::vector<std::shared_ptr<chunkType>>>    chunkLists;
} writeInfoType;

}
namespace datablock
{
// ********* DATA RETRIEVAL BLOCKS *********
// remember they are 32 bit aligned !

typedef struct atomHeaderBlock
{
    uint32_t    size32; // (big endian) size of atom 32 bit
    char        key[4]; // (4 char) FourCC key of atom
    uint64_t    size64; // (big endian) size of atom 64 bit
} atomHeaderBlock;

typedef struct atomTableBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint32_t    numberOfEntries;        // (32-bit integer) number of sample descriptions that follow
} atomTableBlock;

typedef struct stszTableBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint32_t    defaultSampleSize;             // if zero, all samples have different size.
    uint32_t    numberOfEntries;        // number of sample descriptions that follow
} stszTableBlock;

typedef struct ftypDataBlock
{
    char        majorBrand[4];
    uint32_t    version;
} ftypDataBlock;

typedef struct uuidDataBlock
{
    char        name[4];
} uuidDataBlock;

typedef struct mdhdDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint32_t    creationTime;
    uint32_t    modificationTime;
    uint32_t    timeScale;          // time units per second
    uint32_t    duration;           // amount of timeScale units
    uint16_t    language;
    uint16_t    quality;
} mdhdDataBlock;

typedef struct mvhdDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint32_t    creationTime;
    uint32_t    modificationTime;
    uint32_t    timeScale;          // time units per second
    uint32_t    duration;           // amount of timeScale units
    uint32_t    preferredRate;      // fixed point
    uint16_t    preferredVolume;    // fixed point
    uint8_t     reserved[10];
    uint32_t    matrix[3][3];
    uint32_t    previewTime;
    uint32_t    previewDuration;
    uint32_t    posterTime;
    uint32_t    selectionTime;
    uint32_t    selectionDuration;
    uint32_t    currentTime;
    uint32_t    nextTrackID;
} mvhdDataBlock;

typedef struct vmhdDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint16_t    graphicsMode;
    uint16_t    opColorR;
    uint16_t    opColorG;
    uint16_t    opColorB;
} vmhdDataBlock;

typedef struct smhdDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint16_t    balance;
    uint16_t    reserved;
} smhdDataBlock;

typedef struct gminDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint16_t    graphicsMode;
    uint16_t    opColorR;
    uint16_t    opColorG;
    uint16_t    opColorB;
    uint16_t    balance;
    uint16_t    reserved;
} gminDataBlock;

typedef struct hdlrDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    char        componentType[4];
    char        componentSubType[4];
    uint32_t    componentManufacturer;
    uint32_t    componentFlags;
    uint32_t    componentFlagsMask;
} hdlrDataBlock;

typedef struct tkhdDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint32_t    creationTime;
    uint32_t    modificationTime;
    uint32_t    trackID;
    uint32_t    reservedA;
    uint32_t    duration;           // the sum of the durations of all of the track’s edits.
                                    // if there is no edit list, then the duration is
                                    // the sum of the sample durations, converted into the
                                    // movie timescale
    uint8_t     reservedB[8];
    uint16_t    layer;
    uint16_t    alternateGroup;
    uint16_t    volume;             // fixed point
    uint16_t    reservedC;
    uint32_t    matrix[3][3];
    uint32_t    trackWidth;
    uint32_t    trackHeight;
} tkhdDataBlock;

typedef struct tcmiDataBlock
{
    uint8_t     version;
    uint8_t     flag[3];
    uint16_t    textFont;
    uint16_t    textFace;
    uint16_t    textSize;
    uint16_t    reserved;
    uint16_t    textColorR;
    uint16_t    textColorG;
    uint16_t    textColorB;
    uint16_t    backgroundColorR;
    uint16_t    backgroundColorG;
    uint16_t    backgroundColorB;
} tcmiDataBlock;

typedef struct stsdEntryDataBlock
{
    uint32_t    size;
    char        dataFormat[4];          // format type FourCC
    uint8_t     reserved[6];            // reserved and set to zero
    uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                        // with samples that use this sample description. Data references are stored in data reference atoms
} stsdEntryDataBlock;

typedef struct elstEntryDataBlock
{
    uint32_t    duration;
    uint32_t    mediaTime;
    uint32_t    mediaRate;
} elstEntryDataBlock;

typedef struct drefTypeBlock
{
    uint8_t     version;
    uint8_t     flag[3];
} drefTypeBlock;

}

}

#endif