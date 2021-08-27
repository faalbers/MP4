#include "hdlr.hpp"
#include <iostream>

MP4::hdlr::hdlr(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct hdlrDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        char        componentType[4];
        char        componentSubType[4];
        uint32_t    componentManufacturer;
        uint32_t    componentFlags;
        uint32_t    componentFlagsMask;
    } hdlrDataBlock;

    // get data
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("hdlr atom can not parse file: "+filePath_);
    hdlrDataBlock hdlrData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &hdlrData, sizeof(hdlrData));
    componentType = std::string(hdlrData.componentType).substr(0,4);
    componentSubType = std::string(hdlrData.componentSubType).substr(0,4);
    uint8_t stringCount;
    fileStream.read((char *) &stringCount, sizeof(stringCount));
    char name[stringCount];
    fileStream.read((char *) name, stringCount);
    componentName = std::string(name).substr(0, stringCount);
    fileStream.close();
}

void MP4::hdlr::printData(int level)
{
    if ( level == -1 ) level = 0;
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string pathIndent = std::string(level*5, ' ');
    std::string dataIndent = std::string(level*5+(levelCount-1)*5+1, ' ');
    std::cout << pathIndent << path_ << " (Handler Reference Atom)" << std::endl;
    std::cout << dataIndent << "componentType   : " << componentType << std::endl;
    std::cout << dataIndent << "componentSubType: " << componentSubType << std::endl;
    std::cout << dataIndent << "componentName   : " << componentName << std::endl;
}

std::string MP4::hdlr::key = "hdlr";

