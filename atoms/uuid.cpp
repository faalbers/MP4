#include "uuid.hpp"
#include <iostream>

MP4::uuid::uuid(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mdhd atom can not parse file: "+filePath_);
    fileStream.seekg(fileDataPos_, fileStream.beg);
    datablock::uuidDataBlock uuidData;
    fileStream.read((char *) &uuidData, sizeof(uuidData));

    name = std::string(uuidData.name).substr(0,4);

    fileStream.close();
}

void MP4::uuid::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "name :" << name << std::endl;
}

void MP4::uuid::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::uuid::key = "uuid";

