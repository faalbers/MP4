#include "dref.hpp"
#include "../atoms.hpp"
#include <iostream>
#include "drefEntry.hpp"

MP4::dref::dref(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct drefDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    numberOfEntries;        // (32-bit integer) number of sample descriptions that follow
    } drefDataBlock;
    typedef struct drefEntryDataBlock
    {
        uint32_t    size;
        char        dataFormat[4];          // format type FourCC
        uint8_t     reserved[6];            // reserved and set to zero
        uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                            // with samples that use this sample description. Data references are stored in data reference atoms
    } drefEntryDataBlock;

    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("dref atom can not parse file: "+filePath);
    drefDataBlock drefData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &drefData, sizeof(drefData));
    drefData.numberOfEntries = _byteswap_ulong(drefData.numberOfEntries);
    auto index = drefData.numberOfEntries;
    drefEntryDataBlock dataReferenceBlock;
    do {
        auto drefEntryAtom = std::make_shared<drefEntry>(filePath, fileStream.tellg());
        drefTable.push_back(drefEntryAtom->reference);
        fileStream.seekg(drefEntryAtom->fileNextPos_, fileStream.beg);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::dref::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Atom)" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (data reference file)\n";
    for ( auto entry : drefTable ) {
        if ( entry == "" )
            std::cout << dataIndent << "[" << index << "] ( this file )" << std::endl;
        else
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" <<std::endl;
        index++;
    }
}

void MP4::dref::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::dref::key = "dref";

