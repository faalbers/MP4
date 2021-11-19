#include "stts.hpp"
#include <iostream>

MP4::stts::stts(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock sttsData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &sttsData, sizeof(sttsData));
    sttsData.numberOfEntries = XXH_swap32(sttsData.numberOfEntries);
    auto index = sttsData.numberOfEntries;
    uint32_t ID = 1;
    do {
        std::vector<uint32_t> sttsEntry;
        uint32_t val;
        fileStream->read((char*) &val, sizeof(val));
        sttsEntry.push_back(XXH_swap32(val));
        fileStream->read((char*) &val, sizeof(val));
        sttsEntry.push_back(XXH_swap32(val));
        sttsTable[ID] = sttsEntry;
        index--;
        ID++;
    } while ( index > 0);
}

MP4::stts::stts(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    auto track = build->getTrack();
    uint32_t lastDuration;
    uint32_t sampleCount = 0;
    uint32_t ID = 1;
    for ( auto sample : track->samples ) {
        if ( sample.first == 1 ) {
            lastDuration = sample.second.duration;
            sampleCount++;
            continue;
        }
        if ( sample.second.duration == lastDuration ) {
            sampleCount++;
            continue;
        }
        std::vector<uint32_t> sttsEntry;
        sttsEntry.push_back(sampleCount);
        sttsEntry.push_back(lastDuration);
        sampleCount = 1;
        lastDuration = sample.second.duration;
        sttsTable[ID] = sttsEntry;
        ID++;
    }
    std::vector<uint32_t> sttsEntry;
    sttsEntry.push_back(sampleCount);
    sttsEntry.push_back(lastDuration);
    sttsTable[ID] = sttsEntry;
}

void MP4::stts::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Time-To-Sample Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (sampleCount , sampleDuration)\n";
    for ( auto entry : sttsTable )
        std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second[0] << ", " << entry.second[1] << " )" << std::endl;
}

std::string MP4::stts::getKey()
{
    return key;
}

void MP4::stts::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock sttsData;

    // default settings
    sttsData.version.version = 0;
    sttsData.version.flag[0] = 0;
    sttsData.version.flag[1] = 0;
    sttsData.version.flag[2] = 0;

    // data settings
    sttsData.numberOfEntries = XXH_swap32((uint32_t) sttsTable.size());

    fileWrite->write((char*) &sttsData, sizeof(sttsData));

    // write table
    uint32_t val;
    for ( auto entry : sttsTable ) {
        val = XXH_swap32(entry.second[0]);
        fileWrite->write((char*) &val, sizeof(val));
        val = XXH_swap32(entry.second[1]);
        fileWrite->write((char*) &val, sizeof(val));
    }
}

const std::string MP4::stts::key = "stts";

