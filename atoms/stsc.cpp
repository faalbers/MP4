#include "stsc.hpp"
#include <iostream>

MP4::stsc::stsc(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock stscData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &stscData, sizeof(stscData));
    stscData.numberOfEntries = XXH_swap32(stscData.numberOfEntries);
    auto index = stscData.numberOfEntries;
    uint32_t ID = 1;
    do {
        std::vector<uint32_t> stscEntry;
        uint32_t val;
        fileStream->read((char*) &val, sizeof(val));
        stscEntry.push_back(XXH_swap32(val));
        fileStream->read((char*) &val, sizeof(val));
        stscEntry.push_back(XXH_swap32(val));
        fileStream->read((char*) &val, sizeof(val));
        stscEntry.push_back(XXH_swap32(val));
        stscTable[ID] = stscEntry;
        index--;
        ID++;
    } while ( index > 0);
}

MP4::stsc::stsc(std::shared_ptr<atomBuild> build)
    : atom(build)
{
}

void MP4::stsc::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample-To-Chunk Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (first chunk , samples per chunk, sample description ID)\n";
    if ( fullLists || (!fullLists && stscTable.size() <= 6) ) {
        for ( auto entry : stscTable ) {
            std::cout << dataIndent
            << "[" << entry.first << "] ( " << entry.second[0]
            << ", " << entry.second[1] << ", " << entry.second[2] << " )"
            << std::endl;
        }
    } else {
        for ( uint32_t index = 1 ; index <= 3; index++ ) {
            std::cout << dataIndent
            << "  [" << index << "] ( " << stscTable[index][0]
            << ", " << stscTable[index][1]
            << ", " << stscTable[index][2] << " )"
            << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        uint32_t tableSize = (uint32_t) stscTable.size();
        for ( uint32_t index = tableSize-2 ; index <= tableSize; index++ ) {
            std::cout << dataIndent
            << "  [" << index << "] ( " << stscTable[index][0]
            << ", " << stscTable[index][1]
            << ", " << stscTable[index][2] << " )"
            << std::endl;
        }
    }
}

void MP4::stsc::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::stsc::getKey()
{
    return key;
}

void MP4::stsc::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock stscData;

    // default settings
    stscData.version.version = 0;
    stscData.version.flag[0] = 0;
    stscData.version.flag[1] = 0;
    stscData.version.flag[2] = 0;


    // forced to value, check later if this needs a copy of original track

    stscData.numberOfEntries = XXH_swap32((uint32_t) 1);

    fileWrite->write((char*) &stscData, sizeof(stscData));

    uint32_t val = XXH_swap32((uint32_t) 1);
    fileWrite->write((char*) &val, sizeof(val));
    fileWrite->write((char*) &val, sizeof(val));
    fileWrite->write((char*) &val, sizeof(val));
}

std::string MP4::stsc::key = "stsc";

