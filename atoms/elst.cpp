#include "elst.hpp"
#include <iostream>

MP4::elst::elst(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("elst atom can not parse file: "+filePath_);
    datablock::atomTableBlock elstData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &elstData, sizeof(elstData));
    elstData.numberOfEntries = XXH_swap32(elstData.numberOfEntries);
    auto index = elstData.numberOfEntries;
    datablock::elstEntryDataBlock elstEntryBlock;
    do {
        elstEntryType elstEntry;
        fileStream.read((char *) &elstEntryBlock, sizeof(elstEntryBlock));
        elstEntry.duration = XXH_swap32(elstEntryBlock.duration);
        elstEntry.mediaTime = XXH_swap32(elstEntryBlock.mediaTime);
        elstEntryBlock.mediaRate = XXH_swap32(elstEntryBlock.mediaRate);
        elstEntry.mediaRate = (float)elstEntryBlock.mediaRate / (float)(1 << 16);
        elstTable.push_back(elstEntry);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::elst::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Edit List Atom) ["<< headerSize_ << "]" << std::endl;
    int index = 1;
    for ( auto entry : elstTable ) {
        std::cout << dataIndent << "Entry [" << index << "]" << std::endl;
        std::cout << dataIndent << "  duration : " << entry.duration << std::endl;
        std::cout << dataIndent << "  mediaTime: " << entry.mediaTime << std::endl;
        std::cout << dataIndent << "  mediaRate: " << entry.mediaRate << std::endl;
        index++;
    }
}

void MP4::elst::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::elst::key = "elst";

