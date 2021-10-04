#include "stsd.hpp"
#include <iostream>

MP4::stsd::stsd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsd atom can not parse file: "+filePath_);
    datablock::atomTableBlock stsdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stsdData, sizeof(stsdData));
    stsdData.numberOfEntries = XXH_swap32(stsdData.numberOfEntries);
    uint32_t ID = 1;
    datablock::stsdEntryDataBlock sampleDescriptionBlock;
    do {
        stsdEntryType stsdEntry;
        fileStream.read((char *) &sampleDescriptionBlock, sizeof(sampleDescriptionBlock));
        auto size = XXH_swap32(sampleDescriptionBlock.size);
        stsdEntry.dataFormat = std::string(sampleDescriptionBlock.dataFormat).substr(0,4);
        stsdEntry.dataReferenceIndex = XXH_swap16(sampleDescriptionBlock.dataReferenceIndex);

        stsdTable[ID] = stsdEntry;
        
        // not sure what is in the tail of the stsd entry
        // visualize it with a string
        auto tailSize = size - sizeof(sampleDescriptionBlock);
        char tail[200];
        if ( tailSize > 0 && tailSize <= 200 ) {
            fileStream.read((char *) tail, tailSize);
            stsdTable[ID].extendedData = std::string(tail).substr(0,tailSize);
        }
    
        ID++;
    } while ( ID <= stsdData.numberOfEntries );
    fileStream.close();
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

std::string MP4::stsd::key = "stsd";

