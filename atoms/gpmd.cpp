#include "gpmd.hpp"
#include <iostream>

MP4::gpmd::gpmd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("gpmd atom can not parse file: "+filePath_);
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &version, sizeof(version));
    version = XXH_swap32(version);
    fileStream.close();
}

void MP4::gpmd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (GoPro MetaData Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "version : " << version << std::endl;
}

void MP4::gpmd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::gpmd::key = "gpmd";

