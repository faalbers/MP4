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

void MP4::stts::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    //writeData_(fileWrite, writeInfo);
    //return;

    uint32_t movieTimeScale;
    uint32_t movieDuration;
    uint32_t trackTimeScale;
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>()) {
        movieTimeScale = mvhd->timeScale;
        movieDuration = mvhd->duration;
    }
    for ( auto mdhd : trakAtom_->getTypeAtoms<mdhd>()) trackTimeScale = mdhd->timeScale;

    // convert video time to track time
    auto timeScaleMult = (double) movieTimeScale / trackTimeScale;
    auto timeScaled = (uint32_t) (movieDuration / timeScaleMult);

    uint32_t totalDuration = 0;
    uint32_t sampleCount = 0;
    for ( auto entry : sttsTable ) {
        totalDuration += entry[0] * entry[1];
        sampleCount += entry[0];
    }
    auto lastSampleCount = sttsTable.back()[0];
    auto lastSampleDuration = sttsTable.back()[1];

    auto durationDiff = (int64_t) timeScaled - (int64_t) totalDuration;
    auto lastDurationchange = (int64_t) lastSampleDuration + durationDiff;

    // make new sttsTable
    auto sttsTableNew = sttsTable;

    if ( lastDurationchange != (int64_t) lastSampleDuration ) {
        if ( lastDurationchange >= 0 ) {
            if ( lastSampleCount > 1 ) {
                // reduce last sample count and add new entry to sttsTable
                sttsTableNew.back()[0] -= 1;
                std::vector<uint32_t> newEntry;
                newEntry.push_back(1);
                newEntry.push_back((uint32_t) lastDurationchange);
                sttsTableNew.push_back(newEntry);
            } else {
                sttsTableNew.back()[1] = (uint32_t) lastDurationchange;
            }
        } else {
            // reduce samples ... REMEMBER to reduce samples in stsz and update chunks
            // will be commented out till that is done also
            // table needs to be created from scratch
            sttsTableNew.clear();
            uint32_t totalDuration = 0;
            uint32_t totalCount = 0;
            for ( auto entry : sttsTable) {
                uint32_t entryDuration = 0;
                uint32_t entryCount;
                for ( entryCount = 0; entryCount < entry[0]; entryCount++ ) {
                    totalDuration += entry[1];
                    if ( totalDuration > timeScaled ) break;
                }
                std::vector<uint32_t> newEntry;
                newEntry.push_back(entryCount);
                newEntry.push_back(entry[1]);
                sttsTableNew.push_back(newEntry);
                if ( totalDuration > timeScaled ) {
                    totalDuration -= entry[1];
                    break;
                }
            }
            std::vector<uint32_t> newEntry;
            newEntry.push_back(1);
            newEntry.push_back(timeScaled - totalDuration);
            sttsTableNew.push_back(newEntry);
        }
    }

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeAtomDataToFile_ can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);
    datablock::atomTableBlock sttsData;
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read((char *) &sttsData, sizeof(sttsData));
    fileRead.close();
    
    auto sttsTableSize = sttsTableNew.size() + ((stts *) appendAtom)->sttsTable.size();
    sttsData.numberOfEntries = _byteswap_ulong((uint32_t) sttsTableSize);

    fileWrite.write((char *) &sttsData, sizeof(sttsData));

    for ( auto entry : sttsTableNew )
        for ( auto val : entry ) {
            val = _byteswap_ulong(val);
            fileWrite.write((char *) &val, sizeof(val));
        }
    for ( auto entry : ((stts *) appendAtom)->sttsTable )
        for ( auto val : entry ) {
            val = _byteswap_ulong(val);
            fileWrite.write((char *) &val, sizeof(val));
        }

    //writeData_(fileWrite, writeInfo);
}

void MP4::stts::createData(splunkType &splunk)
{
    //createData_(splunk);

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

