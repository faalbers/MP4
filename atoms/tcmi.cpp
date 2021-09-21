#include "tcmi.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::tcmi::tcmi(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
    , fontName("")
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("tcmi atom can not parse file: "+filePath_);
    fileStream.seekg(fileDataPos_, fileStream.beg);

    datablock::tcmiDataBlock tcmiData;
    fileStream.read((char *) &tcmiData, sizeof(tcmiData));
    textFont = _byteswap_ushort(tcmiData.textFont);
    textFace = _byteswap_ushort(tcmiData.textFace);
    textSize = _byteswap_ushort(tcmiData.textSize);
    textColorR = _byteswap_ushort(tcmiData.textColorR);
    textColorG = _byteswap_ushort(tcmiData.textColorG);
    textColorB = _byteswap_ushort(tcmiData.textColorB);
    backgroundColorR = _byteswap_ushort(tcmiData.backgroundColorR);
    backgroundColorG = _byteswap_ushort(tcmiData.backgroundColorG);
    backgroundColorB = _byteswap_ushort(tcmiData.backgroundColorB);

    if ( (fileNextPos_ - fileStream.tellg()) > 0 ) {
        uint8_t pascalLength;
        fileStream.read((char *) &pascalLength, sizeof(pascalLength));
        auto bufferSize = (size_t) pascalLength;
        char *buffer = new char[bufferSize];
        fileStream.read(buffer, bufferSize);
        delete[] buffer;
        fontName = std::string(buffer).substr(0, bufferSize);
    }

    fileStream.close();

}

void MP4::tcmi::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Information Atom) ["<< headerSize_ << "]" << std::endl;
    if ( fontName == "" )
        std::cout << dataIndent << "textFont: " << (int) textFont << std::endl;
    else
        std::cout << dataIndent << "textFont: " << fontName << std::endl;
    std::cout << dataIndent << "textSize: " << (int) textSize << std::endl;
    std::cout << std::hex;
    std::cout << dataIndent << "textFace: 0x" << textFace << std::endl;
    std::cout << dataIndent << "textColorRGB      : "
        << " 0x" << textColorR << " 0x" << textColorG << " 0x" << textColorB << std::endl;
    std::cout << dataIndent << "backgroundColorRGB: "
        << " 0x" << backgroundColorR << " 0x" << backgroundColorG << " 0x" << backgroundColorB << std::endl;
    std::cout << std::dec;
}

void MP4::tcmi::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::tcmi::key = "tcmi";

