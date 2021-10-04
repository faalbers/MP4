#include "stsc.hpp"
#include <iostream>

MP4::stsc::stsc(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsc atom can not parse file: "+filePath_);
    datablock::atomTableBlock stscData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stscData, sizeof(stscData));
    stscData.numberOfEntries = XXH_swap32(stscData.numberOfEntries);
    auto index = stscData.numberOfEntries;
    uint32_t ID = 1;
    do {
        std::vector<uint32_t> stscEntry;
        uint32_t val;
        fileStream.read((char *) &val, sizeof(val));
        stscEntry.push_back(XXH_swap32(val));
        fileStream.read((char *) &val, sizeof(val));
        stscEntry.push_back(XXH_swap32(val));
        fileStream.read((char *) &val, sizeof(val));
        stscEntry.push_back(XXH_swap32(val));
        stscTable[ID] = stscEntry;
        index--;
        ID++;
    } while ( index > 0);
    fileStream.close();
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

std::string MP4::stsc::key = "stsc";

