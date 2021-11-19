#include "ctts.hpp"
#include <iostream>

MP4::ctts::ctts(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock cttsData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &cttsData, sizeof(cttsData));
    cttsData.numberOfEntries = XXH_swap32(cttsData.numberOfEntries);
    auto index = cttsData.numberOfEntries;
    uint32_t ID = 1;
    do {
        std::vector<uint32_t> cttsEntry;
        uint32_t val;
        fileStream->read((char*) &val, sizeof(val));
        cttsEntry.push_back(XXH_swap32(val));
        fileStream->read((char*) &val, sizeof(val));
    cttsEntry.push_back(XXH_swap32(val));
        cttsTable[ID] = cttsEntry;
        index--;
        ID++;
    } while ( index > 0);
}

MP4::ctts::ctts(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    auto track = build->getTrack();
    uint32_t cttsID = 1;
    uint32_t lastCompOffset = track->samples[1].compositionOffset;
    uint32_t sampleCount = 0;
    for ( auto sample : track->samples ) {
        if ( sample.second.compositionOffset != lastCompOffset ) {
            std::vector<uint32_t> entry;
            entry.push_back(sampleCount);
            entry.push_back(lastCompOffset);
            cttsTable[cttsID] = entry;

            lastCompOffset = sample.second.compositionOffset;
            sampleCount = 0;
            cttsID++;
        }
        sampleCount++;
    }
    std::vector<uint32_t> entry;
    entry.push_back(sampleCount);
    entry.push_back(lastCompOffset);
    cttsTable[cttsID] = entry;
}

void MP4::ctts::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Composition Offset Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (sampleCount , compositionOffset)\n";
    if ( fullLists || (!fullLists && cttsTable.size() <= 6) ) {
        for ( auto entry : cttsTable ) {
            std::cout << dataIndent
            << "[" << entry.first << "] ( " << entry.second[0] << ", " << entry.second[1]  << " )"
            << std::endl;
        }
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent
            << "[" << index << "] ( " << cttsTable.at(index)[0] << ", " << cttsTable.at(index)[1] << " )"
            << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) cttsTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent
            << "[" << index << "] ( " << cttsTable.at(index)[0] << ", " << cttsTable.at(index)[1] << " )"
            << std::endl;
        }
    }
}

std::string MP4::ctts::getKey() const
{
    return key;
}

void MP4::ctts::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock cttsData;

    // default settings
    cttsData.version.version = 0;
    cttsData.version.flag[0] = 0;
    cttsData.version.flag[1] = 0;
    cttsData.version.flag[2] = 0;


    // forced to value, check later if this needs a copy of original track

    cttsData.numberOfEntries = XXH_swap32((uint32_t) cttsTable.size());

    fileWrite->write((char*) &cttsData, sizeof(cttsData));

    // write table
    uint32_t val;
    for ( auto entry : cttsTable ) {
        val = XXH_swap32(entry.second[0]);
        fileWrite->write((char*) &val, sizeof(val));
        val = XXH_swap32(entry.second[1]);
        fileWrite->write((char*) &val, sizeof(val));
    }

}

const std::string MP4::ctts::key = "ctts";

