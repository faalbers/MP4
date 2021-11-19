#ifndef MP4_STSZ_H
#define MP4_STSZ_H
#include <string>
#include <memory>
#include <map>
#include "atom.hpp"

/*
SAMPLE SIZE ATOM:
You use sample size atoms to specify the size of each sample in the media.
The sample size atom contains the sample count and a table giving the size of each sample.
This allows the media data itself to be unframed.
The total number of samples in the media is always indicated in the sample count.
If the default size is indicated, then no table follows.
*/

namespace MP4
{

class stsz : public atom
{
public:
    stsz(atomParse& parse);
    stsz(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string      key;
    uint32_t                defaultSampleSize;
    // stszTable[sampleID] = sampleSize;
    std::map<uint32_t, uint32_t>   stszTable;

private:
    typedef struct tableBlock
    {
        versionBlock    version;
        uint32_t        defaultSampleSize;             // if zero, all samples have different size.
        uint32_t        numberOfEntries;        // number of sample descriptions that follow
    } tableBlock;


};

}
#endif