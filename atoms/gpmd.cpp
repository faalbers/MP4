#include "gpmd.hpp"
#include <iostream>

MP4::gpmd::gpmd(atomParse parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &version, sizeof(version));
    version = XXH_swap32(version);
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

