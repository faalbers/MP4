#include "smhd.hpp"
#include <iostream>

MP4::smhd::smhd(atomBuild &build)
    : atom(build)
{
    typedef struct dataBlock
    {
        versionBlock    version;
        uint16_t        balance;
        uint16_t        reserved;
    } dataBlock;

    auto fileStream = build.getFileStream();

    dataBlock smhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &smhdData, sizeof(smhdData));
    balance = (float)XXH_swap16(smhdData.balance) / (float)(1 << 8);
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

