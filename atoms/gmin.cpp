#include "gmin.hpp"
#include <iostream>

MP4::gmin::gmin(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("gmin atom can not parse file: "+filePath_);
    datablock::gminDataBlock gminData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &gminData, sizeof(gminData));
    graphicMode = XXH_swap16(gminData.graphicsMode);
    opColorR = XXH_swap16(gminData.opColorR);
    opColorG = XXH_swap16(gminData.opColorG);
    opColorB = XXH_swap16(gminData.opColorB);
    balance = (float)XXH_swap16(gminData.balance) / (float)(1 << 8);
    fileStream.close();
}

void MP4::gmin::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Base Media Info Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << std::hex;
    std::cout << dataIndent << "graphicMode: 0x" << graphicMode << std::endl;
    std::cout << dataIndent << "opColorR   : 0x" << opColorR << std::endl;
    std::cout << dataIndent << "opColorG   : 0x" << opColorG << std::endl;
    std::cout << dataIndent << "opColorB   : 0x" << opColorB << std::endl;
    std::cout << std::dec;
    std::cout << dataIndent << "balance    : " << balance << std::endl;
}

void MP4::gmin::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::gmin::key = "gmin";

