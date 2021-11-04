#include "mdat.hpp"
#include <iostream>
#include <map>

MP4::mdat::mdat(atomParse &parse)
    : atom(parse)
{
    sampleDataPos = fileDataPos_;
    sampleDataSize = dataSize_;
}

MP4::mdat::mdat(std::shared_ptr<atomBuild> build)
    : atom(build)
    , sampleDataPos(0)
    , sampleDataSize(0)
{
    headerSize_ = 8;
    path_ = parentPath_ + key;
}

void MP4::mdat::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "filePosition of sample data : " << sampleDataPos << std::endl;
    std::cout << dataIndent << "Size of sample data in bytes: " << sampleDataSize << std::endl;
}

void MP4::mdat::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::mdat::writeHeader(std::ofstream &fileWrite)
{
    //writeHeader_(fileWrite, key, false);
    writeHeader_(fileWrite, "lulu", false);
}

std::string MP4::mdat::key = "mdat";

