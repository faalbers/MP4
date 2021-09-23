#include "dref.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::dref::dref(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // handle data 
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("dref atom can not parse file: "+filePath_);
    datablock::atomTableBlock drefData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &drefData, sizeof(drefData));
    atomBuild.filePos = fileStream.tellg();
    fileStream.close();

    drefData.numberOfEntries = XXH_swap32(drefData.numberOfEntries);
    uint32_t index = 1;
    atomBuild.parentPath += "dref/";
    do {
        dataReferences.push_back(makeAtom_(atomBuild));
        index++;
    } while ( index <= drefData.numberOfEntries );
}

void MP4::dref::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Atom) ["<< headerSize_ << "]" << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] ( Data Reference Type Atom )\n";
    for ( auto entry : dataReferences ) {
        std::cout << dataIndent << "[" << index << "] ( ";
        entry->printData();
        index++;
    }
}

void MP4::dref::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::dref::key = "dref";

