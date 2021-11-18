#ifndef MP4_STCO_H
#define MP4_STCO_H
#include <string>
#include <memory>
#include <map>
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
However, be careful whenructing a self-contained QuickTime file with its metadata (movie atom)
at the front because the size of the movie atom affects the chunk offsets to the media data.
*/

namespace MP4
{

class stco : public atom
{
public:
    stco(atomParse& parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);
    
    std::string getKey();

    static std::string      key;
    uint32_t                defaultSampleSize;
    // stcoTable[chunkID] = chunkOffset
    std::map<uint32_t, uint64_t>    stcoTable;
};

}
#endif