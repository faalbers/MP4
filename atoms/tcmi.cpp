#include "tcmi.hpp"
#include <iostream>

MP4::tcmi::tcmi(atomParse& parse)
    : atom(parse)
    , fontName("")
{
    typedef struct dataBlock
    {
        versionBlock    version;
        uint16_t        textFont;
        uint16_t        textFace;
        uint16_t        textSize;
        uint16_t        reserved;
        uint16_t        textColorR;
        uint16_t        textColorG;
        uint16_t        textColorB;
        uint16_t        backgroundColorR;
        uint16_t        backgroundColorG;
        uint16_t        backgroundColorB;
    } dataBlock;

    auto fileStream = parse.getFileStream();

    fileStream->seekg(fileDataPos_, fileStream->beg);

    dataBlock tcmiData;
    fileStream->read((char*) &tcmiData, sizeof(tcmiData));
    textFont = XXH_swap16(tcmiData.textFont);
    textFace = XXH_swap16(tcmiData.textFace);
    textSize = XXH_swap16(tcmiData.textSize);
    textColorR = XXH_swap16(tcmiData.textColorR);
    textColorG = XXH_swap16(tcmiData.textColorG);
    textColorB = XXH_swap16(tcmiData.textColorB);
    backgroundColorR = XXH_swap16(tcmiData.backgroundColorR);
    backgroundColorG = XXH_swap16(tcmiData.backgroundColorG);
    backgroundColorB = XXH_swap16(tcmiData.backgroundColorB);

    if ( (fileNextPos_ - fileStream->tellg()) > 0 ) {
        uint8_t pascalLength;
        fileStream->read((char*) &pascalLength, sizeof(pascalLength));
        auto bufferSize = (size_t) pascalLength;
        char* buffer = new char[bufferSize];
        fileStream->read(buffer, bufferSize);
        delete[] buffer;
        fontName = std::string(buffer).substr(0, bufferSize);
    }


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

std::string MP4::tcmi::getKey() const
{
    return key;
}

const std::string MP4::tcmi::key = "tcmi";

