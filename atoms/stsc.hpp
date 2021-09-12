#ifndef MP4_STSC_H
#define MP4_STSC_H
#include <string>
#include <memory>
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
    stsc(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    std::vector<std::vector<uint32_t>>  stscTable; // (first chunk , samples per chunk, sample description ID)
};

}
#endif