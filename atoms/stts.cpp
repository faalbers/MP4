#include "stts.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stts::stts(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stts atom can not parse file: "+filePath_);
    datablock::atomTableBlock sttsData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &sttsData, sizeof(sttsData));
    sttsData.numberOfEntries = _byteswap_ulong(sttsData.numberOfEntries);
    auto index = sttsData.numberOfEntries;
    do {
        std::vector<uint32_t> sttsEntry;
        uint32_t val;
        fileStream.read((char *) &val, sizeof(val));
        sttsEntry.push_back(_byteswap_ulong(val));
        fileStream.read((char *) &val, sizeof(val));
        sttsEntry.push_back(_byteswap_ulong(val));
        sttsTable.push_back(sttsEntry);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stts::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Time-To-Sample Atom) ["<< headerSize_ << "]" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (sampleCount , sampleDuration)\n";
    for ( auto entry : sttsTable ) {
        std::cout << dataIndent << "[" << index << "] ( " << entry[0] << ", " << entry[1] << " )" << std::endl;
        index++;
    }
}

void MP4::stts::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::stts::createData(splunkType &splunk)
{
    std::vector<uint32_t> sampleDurations;
    auto trackID = trakAtom_->getID();
    for ( auto sample : splunk.samples ) {
        if ( sample.trackID == trackID ) {
            sampleDurations.push_back(sample.duration);
        }
    }
    sampleDurations.push_back(0);

    std::vector<std::vector<uint32_t>> sttsNew;
    uint32_t sampleCounter = 0;
    uint32_t lastDuration = sampleDurations[0];
    for ( auto duration : sampleDurations ) {
        if ( duration != lastDuration) {
            std::vector<uint32_t> entry;
            entry.push_back(sampleCounter);
            entry.push_back(lastDuration);
            sttsNew.push_back(entry);
            sampleCounter = 0;
        }
        sampleCounter++;
        lastDuration = duration;
    }

    datablock::atomTableBlock sttsData;
    sttsData.version = 0;
    sttsData.flag[0] = 0;
    sttsData.flag[1] = 0;
    sttsData.flag[2] = 0;
    sttsData.numberOfEntries = _byteswap_ulong((uint32_t) sttsNew.size());

    splunk.fileWrite->write((char *) &sttsData, sizeof(sttsData));

    uint32_t entryVal;
    for ( auto entry : sttsNew ) {
        entryVal = _byteswap_ulong(entry[0]);
        splunk.fileWrite->write((char *) &entryVal, sizeof(entryVal));
        entryVal = _byteswap_ulong(entry[1]);
        splunk.fileWrite->write((char *) &entryVal, sizeof(entryVal));
    }
}

std::string MP4::stts::key = "stts";

