#ifndef MP4_STSD_H
#define MP4_STSD_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
SAMPLE DESCRIPTION ATOM:
The sample description atom contains a table of sample descriptions.
A media may have one or more sample descriptions, depending upon the number of different
encoding schemes used in the media and on the number of files used to store the data.
The sample-to-chunk atom identifies the sample description for each sample in the media
by specifying the index into this table for the appropriate description.
*/

namespace MP4
{

class stsd : public atom
{
private:
    typedef struct stsdEntryType_
    {
        uint32_t    size;
        std::string dataFormat;             // format type FourCC
        uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                            // with samples that use this sample description. Data references are stored in data reference atoms
        std::string tail;
    } stsdEntryType_;

public:
    stsd(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    std::vector<stsdEntryType_> stsdTable;

};

}
#endif