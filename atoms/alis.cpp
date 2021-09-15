#include "alis.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::alis::alis(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
    , dataInSameFile(false)
{
    std::cout << "alis atom size: " << size_ << std::endl;
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("alis atom can not parse file: "+filePath_);
    datablock::drefTypeBlock drefType;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &drefType, sizeof(drefType));
    fileStream.close();
    if ( drefType.flag[2] == 1) dataInSameFile = true;
    if ( !dataInSameFile )
        throw std::runtime_error("alis data reference should be in same file");
}

void MP4::alis::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Type 'alis' Atom)" << std::endl;
    if (dataInSameFile)
        std::cout << dataIndent << "Data is in same file" << std::endl;
}

void MP4::alis::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::alis::key = "alis";

