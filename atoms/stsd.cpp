#include "stsd.hpp"
#include <iostream>

MP4::stsd::stsd(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock stsdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &stsdData, sizeof(stsdData));
    stsdData.numberOfEntries = XXH_swap32(stsdData.numberOfEntries);
    uint32_t ID = 1;
    entryDataBlock sampleDescriptionBlock;
    do {
        entryType stsdEntry;
        fileStream->read((char*) &sampleDescriptionBlock, sizeof(sampleDescriptionBlock));
        auto size = XXH_swap32(sampleDescriptionBlock.size);
        stsdEntry.dataFormat = std::string(sampleDescriptionBlock.dataFormat).substr(0,4);
        stsdEntry.dataReferenceIndex = XXH_swap16(sampleDescriptionBlock.dataReferenceIndex);

        stsdTable[ID] = stsdEntry;
        
        // not sure what is in the tail of the stsd entry
        // this needs to be handled I'm sure. but just gonna handle it as a char block for now
        auto tailSize = (size_t) size - sizeof(sampleDescriptionBlock);
        if ( tailSize > 0 ) {
            auto buffer = new char[tailSize];
            fileStream->read((char*) buffer, tailSize);
            stsdTable[ID].dataExtended = std::string(buffer,tailSize);
            delete[] buffer;
        }
        ID++;
    } while ( ID <= stsdData.numberOfEntries );
}

MP4::stsd::stsd(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    // for now only handling one entry
    auto track = build->getTrack();
    entryType stsdEntry;
    stsdEntry.dataFormat = track->dataFormat;
    stsdEntry.dataReferenceIndex = track->dataReferenceIndex;
    stsdEntry.dataExtended = track->dataExtended;
    stsdTable[1] = stsdEntry;
}

void MP4::stsd::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Description Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (data format, data reference index, extended data size)\n";
    for ( auto entry : stsdTable ) {
        std::cout << dataIndent << "[" <<  entry.first << "] ( '" << entry.second.dataFormat
        << "', " << entry.second.dataReferenceIndex
        << ", " << entry.second.dataExtended.size() << " )" << std::endl;
        //dataStringViz(entry.second.dataExtended, dataIndent);
    }
}

void MP4::stsd::printHierarchyData(bool fullLists) const
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stsd::getKey()
{
    return key;
}

void MP4::stsd::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();

    tableBlock stsdData;
    
    // default settings
    stsdData.version.version = 0;
    stsdData.version.flag[0] = 0;
    stsdData.version.flag[1] = 0;
    stsdData.version.flag[2] = 0;

    // data settings
    stsdData.numberOfEntries = XXH_swap32((uint32_t) stsdTable.size());

    fileWrite->write((char*) &stsdData, sizeof(stsdData));

    // write table
    for ( auto entry : stsdTable ) {
        entryDataBlock entryData;
        for ( int i = 0; i < 6; i++ ) entryData.reserved[i] = 0;
        memcpy(&entryData.dataFormat, entry.second.dataFormat.c_str(), 4);
        entryData.dataReferenceIndex = XXH_swap16(entry.second.dataReferenceIndex);
        auto size = (uint32_t) (sizeof(entryData) + entry.second.dataExtended.size());
        entryData.size = XXH_swap32(size);
        fileWrite->write((char*) &entryData, sizeof(entryData));
        if ( entry.second.dataExtended.size() > 0 )
            fileWrite->write((char*) entry.second.dataExtended.c_str(), (size_t) entry.second.dataExtended.size());
    }
}

std::set<std::string> MP4::stsd::getDataFormats()
{
    std::set<std::string> dataFormats;
    for ( auto entry : stsdTable ) dataFormats.insert(entry.second.dataFormat);
    return dataFormats;
}

std::string MP4::stsd::key = "stsd";

