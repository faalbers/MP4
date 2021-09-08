#include "mdat.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::mdat::mdat(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    sampleDataPos = fileDataPos_;
    sampleDataSize = dataSize_;
}

void MP4::mdat::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Data Atom)" << std::endl;
    std::cout << dataIndent << "filePosition of sample data : " << sampleDataPos << std::endl;
    std::cout << dataIndent << "Size of sample data in bytes: " << sampleDataSize << std::endl;
}

void MP4::mdat::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::mdat::writeAtomDataToFile(std::ofstream &fileWrite, char *data)
{
    std::cout << dataSize_ << std::endl;
    return;
    
    if ( filePath_ == "" ) return;
    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeAtomDataToFile_ can not parse file: "+filePath_);
    auto bufferSize = (size_t) dataSize_;
    auto buffer = new char[bufferSize];
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read(buffer, (size_t) bufferSize);
    fileWrite.write(buffer, (size_t) bufferSize);
    fileRead.close();
    delete[] buffer;
}

std::string MP4::mdat::key = "mdat";

