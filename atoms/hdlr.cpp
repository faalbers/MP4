#include "hdlr.hpp"
#include <iostream>

MP4::hdlr::hdlr(atomParse parse)
    : atom(parse)
{
    typedef struct dataBlock
    {
        versionBlock    version;
        char            componentType[4];
        char            componentSubType[4];
        uint32_t        componentManufacturer;
        uint32_t        componentFlags;
        uint32_t        componentFlagsMask;
    } dataBlock;

    // get data
    auto fileStream = parse.getFileStream();

    dataBlock hdlrData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &hdlrData, sizeof(hdlrData));
    componentType = std::string(hdlrData.componentType).substr(0,4);
    componentSubType = std::string(hdlrData.componentSubType).substr(0,4);
    uint8_t stringCount;
    fileStream->read((char *) &stringCount, sizeof(stringCount));
    char name[200];
    fileStream->read((char *) name, stringCount);
    componentName = std::string(name).substr(0, stringCount);
}

void MP4::hdlr::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Handler Reference Atom) ["<< headerSize_ << "]" << std::endl;
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

