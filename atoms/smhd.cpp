#include "smhd.hpp"
#include <iostream>

MP4::smhd::smhd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("smhd atom can not parse file: "+filePath_);
    datablock::smhdDataBlock smhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &smhdData, sizeof(smhdData));
    balance = (float)XXH_swap16(smhdData.balance) / (float)(1 << 8);
    fileStream.close();
}

void MP4::smhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sound Media Information Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "balance: " << balance << std::endl;

}

void MP4::smhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::smhd::key = "smhd";

