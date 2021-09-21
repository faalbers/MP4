#include "elst.hpp"
#include "../atoms.hpp"
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
    elstData.numberOfEntries = _byteswap_ulong(elstData.numberOfEntries);
    auto index = elstData.numberOfEntries;
    datablock::elstEntryDataBlock elstEntryBlock;
    do {
        elstEntryType elstEntry;
        fileStream.read((char *) &elstEntryBlock, sizeof(elstEntryBlock));
        elstEntry.duration = _byteswap_ulong(elstEntryBlock.duration);
        elstEntry.mediaTime = _byteswap_ulong(elstEntryBlock.mediaTime);
        elstEntryBlock.mediaRate = _byteswap_ulong(elstEntryBlock.mediaRate);
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
    std::cout << path_ << " (Edit List Atom)" << std::endl;
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

void MP4::elst::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("MP4::elst::appendData can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);
    datablock::atomTableBlock elstData;
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read((char *) &elstData, sizeof(elstData));

    // not sure what to do in this situation
    if ( elstTable.size() > 1 ) throw std::runtime_error("MP4::elst::appendData elstTable larger the 1");

    fileWrite.write((char *) &elstData, sizeof(elstData));

    uint32_t movieTimeScaleA;
    uint32_t movieDurationA;
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>()) {
        movieTimeScaleA = mvhd->timeScale;
        movieDurationA = mvhd->duration;
    }
    uint32_t movieTimeScaleB;
    uint32_t movieDurationB;
    for ( auto mvhd : ((elst *)appendAtom)->moovAtom_->getTypeAtoms<mvhd>()) {
        movieTimeScaleB = mvhd->timeScale;
        movieDurationB = mvhd->duration;
    }

    auto movieDuration = movieDurationA;
    auto timeScaleMult = (double) movieTimeScaleB / movieTimeScaleA;
    movieDuration += (uint32_t) (movieDurationB / timeScaleMult);

    datablock::elstEntryDataBlock elstEntryBlock;
    for ( auto entry : elstTable ) {
        fileRead.read((char *) &elstEntryBlock, sizeof(elstEntryBlock));
        elstEntryBlock.duration = _byteswap_ulong(movieDuration);
        fileWrite.write((char *) &elstEntryBlock, sizeof(elstEntryBlock));
    }
    fileRead.close();

}

/*
void MP4::elst::createData(splunkType &splunk)
{
    //createData_(splunk);

    uint32_t timeScale;
    for ( auto mdhd : trakAtom_->getTypeAtoms<mdhd>() ) timeScale = mdhd->timeScale;
    
    auto timeScaleMult = (double) timeScale / splunk.videoTimeScale;
    auto fullDuration = (uint32_t) (timeScaleMult * (double) splunk.videoDuration);

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("MP4::elst::createData atom can not parse file: "+filePath_);
    datablock::atomTableBlock elstData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &elstData, sizeof(elstData));
    splunk.fileWrite->write((char *) &elstData, sizeof(elstData));
    auto index = elstTable.size();
    datablock::elstEntryDataBlock elstEntryBlock;
    do {
        fileStream.read((char *) &elstEntryBlock, sizeof(elstEntryBlock));
        elstEntryBlock.duration = _byteswap_ulong(fullDuration);
        splunk.fileWrite->write((char *) &elstEntryBlock, sizeof(elstEntryBlock));
        index--;
    } while ( index > 0);
}
*/
std::string MP4::elst::key = "elst";

