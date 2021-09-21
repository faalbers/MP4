#ifndef MP4_STCO_H
#define MP4_STCO_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
CHUNK OFFSET ATOM:
Chunk offset atoms identify the location of each chunk of data in the media’s data stream.
The chunk-offset table gives the index of each chunk into the containing file.
There are two variants, permitting the use of 32-bit (stco) or 64-bit (co64) offsets.
The latter is useful when managing very large movies.
Only one of these variants occurs in any single instance of a sample table atom.
Note that offsets are file offsets, not the offset into any atom within the file (for example, a 'mdat' atom).
This permits referring to media data in files without any atom structure.
However, be careful when constructing a self-contained QuickTime file with its metadata (movie atom)
at the front because the size of the movie atom affects the chunk offsets to the media data.
*/

namespace MP4
{

class stco : public atom
{
public:
    stco(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);
    
    void writeData(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    void appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    
    //void createHeader(splunkType &splunk);
    //void createData(splunkType &splunk);

    static std::string      key;
    uint32_t                defaultSampleSize;
    std::vector<uint64_t>   stcoTable;
};

}
#endif