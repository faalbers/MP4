#include "mdhd.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <chrono>

MP4::mdhd::mdhd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mdhd atom can not parse file: "+filePath_);
    datablock::mdhdDataBlock mdhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &mdhdData, sizeof(mdhdData));
    creationTime = _byteswap_ulong(mdhdData.creationTime);
    modificationTime = _byteswap_ulong(mdhdData.modificationTime);
    timeScale = _byteswap_ulong(mdhdData.timeScale);
    duration = _byteswap_ulong(mdhdData.duration);
    fileStream.close();
}

void MP4::mdhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Header Atom)"<< std::endl;
    std::cout << dataIndent << "timeScale  : " << timeScale << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
}

void MP4::mdhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::mdhd::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    //writeData_(fileWrite, writeInfo);

    uint32_t movieTimeScale;
    uint32_t movieDuration;
    uint32_t trackTimeScale;
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>()) {
        movieTimeScale = mvhd->timeScale;
        movieDuration = mvhd->duration;
    }

    // convert video time to track time
    auto timeScaleMultA = (double) movieTimeScale / timeScale;
    auto timeScaled = (uint32_t) (movieDuration / timeScaleMultA);

    // add append duration
    auto timeScaleMultB = (double) ((mdhd *)appendAtom)->timeScale / timeScale;
    timeScaled += (uint32_t) (((mdhd *)appendAtom)->duration / timeScaleMultB);

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeAtomDataToFile_ can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);
    datablock::mdhdDataBlock mdhdData;
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read((char *) &mdhdData, sizeof(mdhdData));
    fileRead.close();

    mdhdData.duration = _byteswap_ulong(timeScaled);

    fileWrite.write((char *) &mdhdData, sizeof(mdhdData));
}

void MP4::mdhd::createData(splunkType &splunk)
{

    auto timeScaleMult = (double) timeScale / splunk.videoTimeScale;
    auto fullDuration = (uint32_t) (timeScaleMult * (double) splunk.videoDuration);

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("MP4::mdhd::createData atom can not parse file: "+filePath_);
    datablock::mdhdDataBlock mdhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &mdhdData, sizeof(mdhdData));
    mdhdData.duration = _byteswap_ulong(fullDuration);

    splunk.fileWrite->write((char *) &mdhdData, sizeof(mdhdData));

}

std::string MP4::mdhd::key = "mdhd";

