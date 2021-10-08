#include "stsd.hpp"
#include <iostream>

MP4::stsd::stsd(atomBuild &build)
    : atom(build)
{
    typedef struct entryDataBlock
    {
        uint32_t    size;
        char        dataFormat[4];          // format type FourCC
        uint8_t     reserved[6];            // reserved and set to zero
        uint16_t    dataReferenceIndex;     // index of the data reference to use to retrieve data associated
                                            // with samples that use this sample description. Data references are stored in data reference atoms
    } entryDataBlock;

    // handle data 
    auto fileStream = build.getFileStream();

    tableBlock stsdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &stsdData, sizeof(stsdData));
    stsdData.numberOfEntries = XXH_swap32(stsdData.numberOfEntries);
    uint32_t ID = 1;
    entryDataBlock sampleDescriptionBlock;
    do {
        entryType stsdEntry;
        fileStream->read((char *) &sampleDescriptionBlock, sizeof(sampleDescriptionBlock));
        auto size = XXH_swap32(sampleDescriptionBlock.size);
        stsdEntry.dataFormat = std::string(sampleDescriptionBlock.dataFormat).substr(0,4);
        stsdEntry.dataReferenceIndex = XXH_swap16(sampleDescriptionBlock.dataReferenceIndex);

        stsdTable[ID] = stsdEntry;
        
        // not sure what is in the tail of the stsd entry
        // visualize it with a string
        auto tailSize = size - sizeof(sampleDescriptionBlock);
        char tail[200];
        if ( tailSize > 0 && tailSize <= 200 ) {
            fileStream->read((char *) tail, tailSize);
            stsdTable[ID].extendedData = std::string(tail).substr(0,tailSize);
        }
    
        ID++;
    } while ( ID <= stsdData.numberOfEntries );
}

void MP4::stsd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Description Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (data format, data reference index, extended data)\n";
    for ( auto entry : stsdTable ) {
        std::cout << dataIndent << "[" <<  entry.first << "] ( '" << entry.second.dataFormat
        << "', " << entry.second.dataReferenceIndex
        << ", '" << entry.second.extendedData << "' )" << std::endl;
    }
}

void MP4::stsd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::set<std::string> MP4::stsd::getDataFormats()
{
    std::set<std::string> dataFormats;
    for ( auto entry : stsdTable ) dataFormats.insert(entry.second.dataFormat);
    return dataFormats;
}

std::string MP4::stsd::key = "stsd";

