#include "vmhd.hpp"
#include <iostream>

MP4::vmhd::vmhd(atomParse &parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    dataBlock vmhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &vmhdData, sizeof(vmhdData));
/*
    std::cout << (int) vmhdData.version.version
        << " " << (int) vmhdData.version.flag[0]
        << " " << (int) vmhdData.version.flag[1]
        << " " << (int) vmhdData.version.flag[2]
        << std::endl;
*/
    graphicsMode = XXH_swap16(vmhdData.graphicsMode);
    opColorR = XXH_swap16(vmhdData.opColorR);
    opColorG = XXH_swap16(vmhdData.opColorG);
    opColorB = XXH_swap16(vmhdData.opColorB);
}

MP4::vmhd::vmhd(std::shared_ptr<atomBuild> build)
    : atom(build)
    , graphicsMode(build->getGraphicsMode())
    , opColorR(build->getOpColorR())
    , opColorG(build->getOpColorG())
    , opColorB(build->getOpColorB())
{
}

void MP4::vmhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Video Media Information Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << std::hex;
    std::cout << dataIndent << "graphicMode: 0x" << graphicsMode << std::endl;
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

void MP4::vmhd::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock vmhdData;

    // default settings
    vmhdData.version.version = 0;
    vmhdData.version.flag[0] = 0;
    vmhdData.version.flag[1] = 0;
    vmhdData.version.flag[2] = 0;

    // forced to value, check later if this needs a copy of original track

    // data settings
    vmhdData.graphicsMode = XXH_swap16(graphicsMode);
    vmhdData.opColorR = XXH_swap16(opColorR);
    vmhdData.opColorG = XXH_swap16(opColorG);
    vmhdData.opColorB = XXH_swap16(opColorB);

    fileWrite->write((char *) &vmhdData, sizeof(vmhdData));
}

std::string MP4::vmhd::getKey()
{
    return key;
}

std::string MP4::vmhd::key = "vmhd";

