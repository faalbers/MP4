#include "stsc.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stsc::stsc(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // handle data 
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stsc atom can not parse file: "+filePath);
    datablock::atomTableBlock stscData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stscData, sizeof(stscData));
    stscData.numberOfEntries = _byteswap_ulong(stscData.numberOfEntries);
    auto index = stscData.numberOfEntries;
    do {
        std::vector<uint32_t> stscEntry;
        uint32_t val;
        fileStream.read((char *) &val, sizeof(val));
        stscEntry.push_back(_byteswap_ulong(val));
        fileStream.read((char *) &val, sizeof(val));
        stscEntry.push_back(_byteswap_ulong(val));
        fileStream.read((char *) &val, sizeof(val));
        stscEntry.push_back(_byteswap_ulong(val));
        stscTable.push_back(stscEntry);
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stsc::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sample-To-Chunk Atom)" << std::endl;
    size_t index = 1;
    std::cout << dataIndent << "[#] (first chunk , samples per chunk, sample description ID)\n";
    if ( fullLists || (!fullLists && stscTable.size() <= 6) ) {
        for ( auto entry : stscTable ) {
            std::cout << dataIndent
            << "[" << index << "] ( " << entry[0] << ", " << entry[1] << ", " << entry[2] << " )"
            << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent
            << "  [" << index << "] ( " << stscTable[index][0]
            << ", " << stscTable[index][1]
            << ", " << stscTable[index][2] << " )"
            << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = stscTable.size()-3 ; index < stscTable.size(); index++ ) {
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

