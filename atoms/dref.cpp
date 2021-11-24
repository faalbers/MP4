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

    dataReferenceCount = XXH_swap32(drefData.numberOfEntries);
    uint32_t ID = 1;

    int64_t childNextPos;
    do {
        parse.setParentPath(path_+"/");
        auto child = makeAtom_(parse);
        childNextPos = child->fileNextPos_;
        fileStream->seekg(childNextPos, fileStream->beg);
        children_.push_back(child);
        ID++;
    } while ( ID <= dataReferenceCount );
}

void MP4::dref::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "number of entries: " << dataReferenceCount << std::endl;
}

std::string MP4::dref::getKey() const
{
    return key;
}

const std::string MP4::dref::key = "dref";

