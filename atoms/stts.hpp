#ifndef MP4_STTS_H
#define MP4_STTS_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
TIME-TO-SAMPLE ATOM:
These atoms store duration information for a media’s samples, providing a mapping from
a time in a media to the corresponding data sample.
You can determine the appropriate sample for any time in a media by examining the time-to-sample
atom table, which is contained in the time-to-sample atom.
The atom contains a compact version of a table that allows indexing from time to sample number.
Other tables provide sample sizes and pointers from the sample number.
Each entry in the table gives the number of consecutive samples with the same time delta, and
the delta of those samples.
By adding the deltas, a complete time-to-sample map can be built.

The atom contains time deltas: DT(n+1) = DT(n) + STTS(n) where STTS(n)
is the (uncompressed) table entry for sample n and DT is the display time for sample (n).
The sample entries are ordered by time stamps; therefore, the deltas are all nonnegative.
The DT axis has a zero origin; DT(i) = SUM (for j=0 to i-1 of delta(j)), and the sum of
all deltas gives the length of the media in the track (not mapped to the overall time scale, and
not considering any edit list).
The edit list atom provides the initial DT value if it is nonempty (nonzero).
*/

namespace MP4
{

class stts : public atom
{
public:
    stts(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    std::vector<std::vector<uint32_t>>  sttsTable; // (sampleCount , sampleDuration)
};

}
#endif