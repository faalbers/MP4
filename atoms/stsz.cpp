#include "stsz.hpp"
#include <iostream>

MP4::stsz::stsz(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    tableBlock stszData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &stszData, sizeof(stszData));
    defaultSampleSize = XXH_swap32(stszData.defaultSampleSize);
    if ( defaultSampleSize == 0 ) {
        stszData.numberOfEntries = XXH_swap32(stszData.numberOfEntries);
        auto index = stszData.numberOfEntries;
        uint32_t defaultSampleSizeTemp;
        uint32_t ID = 1;
        do {
            fileStream->read((char*) &defaultSampleSizeTemp, sizeof(defaultSampleSizeTemp));
            stszTable[ID] = XXH_swap32(defaultSampleSizeTemp);
            index--;
            ID++;
        } while ( index > 0);
    }
}

MP4::stsz::stsz(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    auto track = build->getTrack();
    defaultSampleSize = 0;
    for ( auto sample : track->samples ) {
        if ( sample.first == 1 ) {
            defaultSampleSize = sample.second.size;
            continue;
        }
        if ( sample.second.size != defaultSampleSize ) {
            defaultSampleSize = 0;
            break;
        }
    }
    if ( defaultSampleSize != 0 ) return;
    for ( auto sample : track->samples ) stszTable[sample.first] = sample.second.size;
}

void MP4::stsz::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample Size Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "defaultSamplSize: " << defaultSampleSize << std::endl;
    if ( stszTable.size() != 0 ) std::cout << dataIndent << "[#] (sample Size)\n";
    if ( fullLists || (!fullLists && stszTable.size() <= 6) ) {
        for ( auto entry : stszTable )
            std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second << " )" << std::endl;
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stszTable.at(index) << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) stszTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stszTable.at(index) << " )" << std::endl;
        }
    }
}

std::string MP4::stsz::getKey() const
{
    return key;
}

void MP4::stsz::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock stszData;

    // default settings
    stszData.version.version = 0;
    stszData.version.flag[0] = 0;
    stszData.version.flag[1] = 0;
    stszData.version.flag[2] = 0;

    // data settings
    stszData.defaultSampleSize = XXH_swap32(defaultSampleSize);
    stszData.numberOfEntries = XXH_swap32((uint32_t) stszTable.size());

    fileWrite->write((char*) &stszData, sizeof(stszData));

    // write table
    uint32_t val;
    for ( auto entry : stszTable ) {
        val = XXH_swap32(entry.second);
        fileWrite->write((char*) &val, sizeof(val));
    }
}

const std::string MP4::stsz::key = "stsz";

