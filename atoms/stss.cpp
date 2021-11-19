#include "stss.hpp"
#include <iostream>

MP4::stss::stss(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    tableBlock stssData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &stssData, sizeof(stssData));
    stssData.numberOfEntries = XXH_swap32(stssData.numberOfEntries);
    auto index = stssData.numberOfEntries;
    uint32_t sampleID;
    uint32_t ID = 1;
    do {
        fileStream->read((char*) &sampleID, sizeof(sampleID));
        stssTable[ID] = XXH_swap32(sampleID);
        index--;
        ID++;
    } while ( index > 0);
}

MP4::stss::stss(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    auto track = build->getTrack();
    uint32_t sttsID = 1;
    for ( auto sample : track->samples ) {
        if ( sample.second.sync ) {
            stssTable[sttsID] = sample.first;
            sttsID++;
        }
    }
}

void MP4::stss::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sync Sample Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (sample ID)\n";
    if ( fullLists || (!fullLists && stssTable.size() <= 6) ) {
        for ( auto entry : stssTable )
            std::cout << dataIndent << "[" << entry.first << "] ( " << entry.second << " )" << std::endl;
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stssTable.at(index) << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) stssTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent << "[" << index << "] ( " << stssTable.at(index) << " )" << std::endl;
        }
    }
}

std::string MP4::stss::getKey() const
{
    return key;
}

void MP4::stss::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock stssData;

    // default settings
    stssData.version.version = 0;
    stssData.version.flag[0] = 0;
    stssData.version.flag[1] = 0;
    stssData.version.flag[2] = 0;

    // data settings
    stssData.numberOfEntries = XXH_swap32((uint32_t) stssTable.size());

    fileWrite->write((char*) &stssData, sizeof(stssData));

    // write table
    uint32_t val;
    for ( auto entry : stssTable ) {
        val = XXH_swap32(entry.second);
        fileWrite->write((char*) &val, sizeof(val));
    }
}

const std::string MP4::stss::key = "stss";

