#include "dref.hpp"
#include "../atoms.hpp"
#include <iostream>
#include "drefEntry.hpp"

MP4::dref::dref(internal::atomBuildType &atomBuild, uint64_t filePos)
    : atom(atomBuild, filePos)
{
    // data blocks for file reading
    typedef struct drefEntryDataBlock
    {
        uint32_t    size;
        char        dataFormat[4];          // format type FourCC
        uint8_t     reserved[6];            // reserved and set to zero
        uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                            // with samples that use this sample description. Data references are stored in data reference atoms
    } drefEntryDataBlock;

    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("dref atom can not parse file: "+filePath_);
    datablock::atomTableBlock drefData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &drefData, sizeof(drefData));
    drefData.numberOfEntries = _byteswap_ulong(drefData.numberOfEntries);
    uint32_t index = 1;
    do {
        auto drefEntryAtom = std::make_shared<drefEntry>(atomBuild, fileStream.tellg());
        drefEntryType drefEntry;
        drefEntry.ID = index;
        drefEntry.reference = drefEntryAtom->reference;
        drefTable.push_back(drefEntry);
        fileStream.seekg(drefEntryAtom->fileNextPos_, fileStream.beg);
        index++;
    } while ( index <= drefData.numberOfEntries );
    fileStream.close();
}

void MP4::dref::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Atom)" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (data reference file)\n";
    for ( auto entry : drefTable ) {
        if ( entry.reference == "" )
            std::cout << dataIndent << "[" << entry.ID << "] ( this file )" << std::endl;
        else
            std::cout << dataIndent << "[" << entry.ID << "] ( " << entry.reference << " )" <<std::endl;
        index++;
    }
}

void MP4::dref::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::dref::key = "dref";

