#include "drefEntry.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::drefEntry::drefEntry(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct drefEntryDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
    } drefEntryDataBlock;

    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("drefEntry atom can not parse file: "+filePath);
    drefEntryDataBlock drefEntryData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &drefEntryData, sizeof(drefEntryData));

    // not sure what data goes into reference
    // visualize it with a string for now
    auto referenceSize = fileNextPos_ - fileStream.tellg();
    if ( referenceSize > 0 ) {
        char referenceChar[referenceSize];
        fileStream.read((char *) referenceChar, referenceSize);
        reference = std::string(referenceChar).substr(0,referenceSize);
    }
    fileStream.close();
}

std::string MP4::drefEntry::key = "drefEntry";

