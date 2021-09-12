#include "hdlr.hpp"
#include <iostream>

MP4::hdlr::hdlr(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos)
    : atom(atomBuild, filePath, filePos)
{
    // get data
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("hdlr atom can not parse file: "+filePath_);
    datablock::hdlrDataBlock hdlrData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &hdlrData, sizeof(hdlrData));
    componentType = std::string(hdlrData.componentType).substr(0,4);
    componentSubType = std::string(hdlrData.componentSubType).substr(0,4);
    uint8_t stringCount;
    fileStream.read((char *) &stringCount, sizeof(stringCount));
    char name[200];
    fileStream.read((char *) name, stringCount);
    componentName = std::string(name).substr(0, stringCount);
    fileStream.close();
}

void MP4::hdlr::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Handler Reference Atom)" << std::endl;
    std::cout << dataIndent << "componentType   : " << componentType << std::endl;
    std::cout << dataIndent << "componentSubType: " << componentSubType << std::endl;
    std::cout << dataIndent << "componentName   : " << componentName << std::endl;
}

void MP4::hdlr::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::hdlr::key = "hdlr";

