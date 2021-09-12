#ifndef MP4_ELST_H
#define MP4_ELST_H
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

class elst : public atom
{
public:
    elst(internal::atomBuildType &atomBuild, uint64_t filePos);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    std::vector<elstEntryType> elstTable;

};

}
#endif