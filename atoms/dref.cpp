#include "dref.hpp"
#include <iostream>

MP4::dref::dref(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock drefData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &drefData, sizeof(drefData));

    drefData.numberOfEntries = XXH_swap32(drefData.numberOfEntries);
    uint32_t ID = 1;
    parse.setParentPath(parse.getParentPath() + "dref/");
    do {
        dataReferences[ID] = makeAtom_(parse);
        ID++;
    } while ( ID <= drefData.numberOfEntries );
}

void MP4::dref::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] ( Data Reference Type Atom )\n";
    for ( auto entry : dataReferences ) {
        std::cout << dataIndent << "[" << entry.first << "] ( ";
        entry.second->printData();
    }
}

std::string MP4::dref::getKey() const
{
    return key;
}

const std::string MP4::dref::key = "dref";

