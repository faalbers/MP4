#ifndef MP4_CTTS_H
#define MP4_CTTS_H
#include <string>
#include <memory>
#include <map>
#include "atom.hpp"

/*
COMPOSITION OFFSET ATOM:
Video samples in encoded formats have a decode order and a presentation order
(also called composition order or display order).
The composition offset atom is used when there are out-of-order video samples.
If the decode and presentation orders are the same, no composition offset atom will be present.
The time-to-sample atom provides both the decode and presentation ordering of the video stream,
and allows calculation of the start and end times.
If video samples are stored out of presentation order, the time-to-sample atom provides the decode
order and the composition offset atom provides the time of presentation for the decoded samples
expressed as a delta on a sample-by-sample basis.
Decode time does not directly imply presentation time when working with out of order video samples.
The ordering is significant.
The composition offset atom contains a sample-by-sample mapping of the decode-to-presentation time.
Each entry in the composition offset table is a time delta from decode to presentation time:
CT(n) = DT(n) + CTTS(n) where CTTS(n) is the (uncompressed) table entry for sample n
DT is the decode time and CT is the composition (or display) time.
The delta expressed in the composition offset table can be positive or negative.
When the time-to-sample atom and the composition offset atom are present, a reader parsing
out-of-order video samples has all the information necessary to calculate the start and end times,
as well as the minimum and maximum offsets between decode time and presentation time.
The sample tables are scanned to obtain these values.
At the last displayed frame, the decode duration is used as presentation duration.
*/

namespace MP4
{

class ctts : public atom
{
public:
    ctts(atomParse &parse);
    ctts(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static std::string  key;
    // cttsTable[cttsID] = decode-to-presentation time
    std::map<uint32_t, std::vector<uint32_t>>  cttsTable; // (sampleCount , compositionOffset)
};

}
#endif