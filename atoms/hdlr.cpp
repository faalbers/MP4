#include "hdlr.hpp"
#include <iostream>

MP4::hdlr::hdlr(atomParse& parse)
    : atom(parse)
{
    // get data
    auto fileStream = parse.getFileStream();

    dataBlock hdlrData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &hdlrData, sizeof(hdlrData));
    componentType = std::string(hdlrData.componentType).substr(0,4);
    componentSubType = std::string(hdlrData.componentSubType).substr(0,4);
    // get sized name string
    uint8_t stringCount;
    fileStream->read((char*) &stringCount, sizeof(stringCount));
    char name[200];
    fileStream->read((char*) name, stringCount);
    componentName = std::string(name).substr(0, stringCount);
}

MP4::hdlr::hdlr(std::shared_ptr<atomBuild> build)
    : atom(build)
    , componentType(build->getComponentType())
    , componentSubType(build->getComponentSubType())
    , componentName(build->getComponentName())
{
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

std::string MP4::hdlr::getKey()
{
    return key;
}

void MP4::hdlr::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock hdlrData;

    // default settings
    hdlrData.version.version = 0;
    hdlrData.version.flag[0] = 0;
    hdlrData.version.flag[1] = 0;
    hdlrData.version.flag[2] = 0;

    // forced to value, check later if this needs a copy of original track
    hdlrData.componentManufacturer = 0;
    hdlrData.componentFlags = 0;
    hdlrData.componentFlagsMask = 0;

    // data settings
    memcpy(&hdlrData.componentType, componentType.c_str(), 4);
    memcpy(&hdlrData.componentSubType, componentSubType.c_str(), 4);

    fileWrite->write((char*) &hdlrData, sizeof(hdlrData));

    // add component name to tail end
    uint8_t nameSize = (uint8_t) componentName.size();
    fileWrite->write((char*) &nameSize, sizeof(nameSize));
    fileWrite->write((char*) componentName.c_str(), (size_t) nameSize);
}

std::string MP4::hdlr::key = "hdlr";

