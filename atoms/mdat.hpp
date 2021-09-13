#ifndef MP4_MDAT_H
#define MP4_MDAT_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
MEDIA DATA ATOM:
the media data atom contains the actual media samples
*/

namespace MP4
{

class mdat : public atom
{
public:
    mdat(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    void writeData(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);

    std::tuple<int64_t, bool>   appendHeader(std::ofstream &fileWrite);
    void                        appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo);

    static std::string  key;
    int64_t             sampleDataPos;
    int64_t             sampleDataSize;

private:
    void extract_(std::ofstream &fileWrite,
        std::vector<std::shared_ptr<chunkType>> &chunkList,
        std::set<uint32_t> &includeTrackIDs);
};

}
#endif