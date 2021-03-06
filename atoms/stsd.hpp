#ifndef MP4_STSD_H
#define MP4_STSD_H
#include <string>
#include <memory>
#include <map>
#include <set>
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
public:
    stsd(atomParse& parse);
    stsd(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile) const;

    std::set<std::string>           getDataFormats();
    std::map<std::string, uint16_t> getReferenceIndices();

    static const std::string    key;
    typedef struct entryType
    {
        std::string dataFormat;         // format type FourCC
        uint16_t    dataReferenceIndex; // index of the data reference to use to retrieve data associated
                                        // with samples that use this sample description.
                                        // Data references are stored in data reference atoms
        std::string dataExtended;
    } entryType;
    // stscTable[stscID] = sampleToChunkEntry
    std::map<uint32_t, entryType> stsdTable;

private:
    typedef struct entryDataBlock
    {
        uint32_t    size;
        char        dataFormat[4];          // format type FourCC
        uint8_t     reserved[6];            // reserved and set to zero
        uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                            // with samples that use this sample description. Data references are stored in data reference atoms
    } entryDataBlock;

};

}
#endif