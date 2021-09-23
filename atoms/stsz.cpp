#include "stsz.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stsz::stsz(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsz atom can not parse file: "+filePath_);
    datablock::stszTableBlock stszData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stszData, sizeof(stszData));
    defaultSampleSize = XXH_swap32(stszData.defaultSampleSize);
    if ( defaultSampleSize == 0 ) {
        stszData.numberOfEntries = XXH_swap32(stszData.numberOfEntries);
        auto index = stszData.numberOfEntries;
        uint32_t defaultSampleSizeTemp;
        do {
            fileStream.read((char *) &defaultSampleSizeTemp, sizeof(defaultSampleSizeTemp));
            stszTable.push_back(XXH_swap32(defaultSampleSizeTemp));
            index--;
        } while ( index > 0);
    }
    fileStream.close();
}

void MP4::stsz::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Size Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "defaultSamplSize: " << defaultSampleSize << std::endl;
    if ( stszTable.size() != 0 ) std::cout << dataIndent << "[#] (sample Size)\n";
    size_t index = 1;
    if ( fullLists || (!fullLists && stszTable.size() <= 6) ) {
        for ( auto entry : stszTable ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stszTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = stszTable.size()-3 ; index < stszTable.size(); index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stszTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stsz::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::stsz::createData(splunkType &splunk)
{
    // no checking of trackID since that is done in the trak level
    
    auto trackID = trakAtom_->getID();

    datablock::stszTableBlock stszData;
    stszData.version = 0;
    stszData.flag[0] = 0;
    stszData.flag[1] = 0;
    stszData.flag[2] = 0;
    stszData.numberOfEntries = 0;

    // check to see if all sizes are the same
    for ( auto sample : splunk.samples ) {
        if ( sample.trackID == trackID ) {
            stszData.defaultSampleSize = sample.size;
            break;
        }
    }
    for ( auto sample : splunk.samples ) {
        if ( sample.trackID == trackID && sample.size != stszData.defaultSampleSize ) {
            stszData.defaultSampleSize = 0;
            stszData.numberOfEntries = XXH_swap32((uint32_t)splunk.tracks[trackID].sampleCount);
            break;
        }
    }

    // write datablock
    splunk.fileWrite->write((char *) &stszData, sizeof(stszData));

    if ( stszData.defaultSampleSize != 0 ) return;

    uint32_t sampleCount = 0;
    for ( auto sample : splunk.samples ) {
        if ( sample.trackID == trackID ) {
            //auto offset = XXH_swap32((uint32_t) sample.filePos);
            auto size = XXH_swap32(sample.size);
            splunk.fileWrite->write((char *) &size, sizeof(size));
            sampleCount++;
        }
    }
}

std::string MP4::stsz::key = "stsz";

