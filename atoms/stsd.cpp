#include "stsd.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stsd::stsd(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct stsdDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    numberOfEntries;        // (32-bit integer) number of sample descriptions that follow
    } stsdDataBlock;
    typedef struct stsdEntryDataBlock
    {
        uint32_t    size;
        char        dataFormat[4];          // format type FourCC
        uint8_t     reserved[6];            // reserved and set to zero
        uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                            // with samples that use this sample description. Data references are stored in data reference atoms
    } stsdEntryDataBlock;

    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsd atom can not parse file: "+filePath);
    stsdDataBlock stsdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stsdData, sizeof(stsdData));
    stsdData.numberOfEntries = _byteswap_ulong(stsdData.numberOfEntries);
    auto index = stsdData.numberOfEntries;
    stsdEntryDataBlock sampleDescriptionBlock;
    do {
        stsdEntryType_ stsdEntry;
        fileStream.read((char *) &sampleDescriptionBlock, sizeof(sampleDescriptionBlock));
        stsdEntry.size = _byteswap_ulong(sampleDescriptionBlock.size);
        stsdEntry.dataFormat = std::string(sampleDescriptionBlock.dataFormat).substr(0,4);
        stsdEntry.dataReferenceIndex = _byteswap_ushort(sampleDescriptionBlock.dataReferenceIndex);
        stsdTable.push_back(stsdEntry);
        
        // not sure what is in the tail of the stsd entry
        // visualize it with a string
        auto tailSize = stsdEntry.size - sizeof(sampleDescriptionBlock);
        if ( tailSize > 0 ) {
            char tail[tailSize];
            fileStream.read((char *) tail, tailSize);
            stsdEntry.extendedData = std::string(tail).substr(0,tailSize);
        }
    
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stsd::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Description Atom)" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (data format, data reference index, extended data)\n";
    for ( auto entry : stsdTable ) {
        std::cout << dataIndent << "[" << index << "] ( '" << entry.dataFormat
        << "', " << entry.dataReferenceIndex
        << ", '" << entry.extendedData << "' )" << std::endl;
        index++;
    }
}

void MP4::stsd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stsd::key = "stsd";

