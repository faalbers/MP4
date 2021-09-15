#ifndef MP4_STSS_H
#define MP4_STSS_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
SYMC SAMPLE ATOM:
The sync sample atom identifies the key frames in the media.
In a media that contains compressed data, key frames define starting points for portions of a
temporally compressed sequence.
The key frame is self-contained—that is, it is independent of preceding frames.
Subsequent frames may depend on the key frame.
The sync sample atom provides a compact marking of the random access points within a stream.
The table is arranged in strictly increasing order of sample number.
If this table is not present, every sample is implicitly a random access point.
The sync sample atom contains a table of sample numbers.
Each entry in the table identifies a sample that is a key frame for the media.
If no sync sample atom exists, then all the samples are key frames.
*/

namespace MP4
{

class stss : public atom
{
public:
    stss(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    void appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo);

    static std::string      key;
    std::vector<uint32_t>   stssTable;
};

}
#endif