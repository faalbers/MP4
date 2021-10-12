#ifndef MP4_STSC_H
#define MP4_STSC_H
#include <string>
#include <memory>
#include <map>
#include "atom.hpp"

/*
SAMPLE-TO-CHUNK ATOM:
As samples are added to a media, they are collected into chunks that allow optimized data access.
A chunk contains one or more samples.
Chunks in a media may have different sizes, and the samples within a chunk may have different sizes.
The sample-to-chunk atom stores chunk information for the samples in a media.
The sample-to-chunk atom contains a table that maps samples to chunks in the media data stream.
By examining the sample-to-chunk atom, you can determine the chunk that contains a specific sample.
*/

namespace MP4
{

class stsc : public atom
{
public:
    stsc(atomParse &parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    // stscTable[stscID] = sampleToChunkEntry
    std::map<uint32_t, std::vector<uint32_t>>  stscTable; // (first chunk , samples per chunk, sample description ID)
};

}
#endif