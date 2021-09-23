#include "vmhd.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::vmhd::vmhd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("vmhd atom can not parse file: "+filePath_);
    datablock::vmhdDataBlock vmhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &vmhdData, sizeof(vmhdData));
    graphicMode = XXH_swap16(vmhdData.graphicsMode);
    opColorR = XXH_swap16(vmhdData.opColorR);
    opColorG = XXH_swap16(vmhdData.opColorG);
    opColorB = XXH_swap16(vmhdData.opColorB);
    fileStream.close();
}

void MP4::vmhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Video Media Information Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << std::hex;
    std::cout << dataIndent << "graphicMode: 0x" << graphicMode << std::endl;
    std::cout << dataIndent << "opColorR   : 0x" << opColorR << std::endl;
    std::cout << dataIndent << "opColorG   : 0x" << opColorG << std::endl;
    std::cout << dataIndent << "opColorB   : 0x" << opColorB << std::endl;
    std::cout << std::dec;
}

void MP4::vmhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::vmhd::key = "vmhd";

