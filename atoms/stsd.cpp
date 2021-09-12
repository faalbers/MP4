#include "stsd.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stsd::stsd(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(atomBuild, filePath, filePos)
{
    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsd atom can not parse file: "+filePath);
    datablock::atomTableBlock stsdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stsdData, sizeof(stsdData));
    stsdData.numberOfEntries = _byteswap_ulong(stsdData.numberOfEntries);
    uint32_t index = 1;
    datablock::stsdEntryDataBlock sampleDescriptionBlock;
    do {
        stsdEntryType stsdEntry;
        fileStream.read((char *) &sampleDescriptionBlock, sizeof(sampleDescriptionBlock));
        stsdEntry.ID = index;
        auto size = _byteswap_ulong(sampleDescriptionBlock.size);
        stsdEntry.dataFormat = std::string(sampleDescriptionBlock.dataFormat).substr(0,4);
        stsdEntry.dataReferenceIndex = _byteswap_ushort(sampleDescriptionBlock.dataReferenceIndex);
        stsdTable.push_back(stsdEntry);
        
        // not sure what is in the tail of the stsd entry
        // visualize it with a string
        auto tailSize = size - sizeof(sampleDescriptionBlock);
        char tail[200];
        if ( tailSize > 0 && tailSize <= 200 ) {
            fileStream.read((char *) tail, tailSize);
            stsdEntry.extendedData = std::string(tail).substr(0,tailSize);
        }
    
        index++;
    } while ( index <= stsdData.numberOfEntries );
    fileStream.close();
}

void MP4::stsd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Description Atom)" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (data format, data reference index, extended data)\n";
    for ( auto entry : stsdTable ) {
        std::cout << dataIndent << "[" <<  entry.ID << "] ( '" << entry.dataFormat
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

