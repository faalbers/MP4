#include "smhd.hpp"
#include <iostream>

MP4::smhd::smhd(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    dataBlock smhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &smhdData, sizeof(smhdData));
    balance = (float)XXH_swap16(smhdData.balance) / (float)(1 << 8);
}

MP4::smhd::smhd(std::shared_ptr<atomBuild> build)
    : atom(build)
    , balance(build->getBalance())
{
}

void MP4::smhd::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sound Media Information Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "balance: " << balance << std::endl;

}

std::string MP4::smhd::getKey() const
{
    return key;
}

void MP4::smhd::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock smhdData;

    // default settings
    smhdData.version.version = 0;
    smhdData.version.flag[0] = 0;
    smhdData.version.flag[1] = 0;
    smhdData.version.flag[2] = 0;

    // forced to value, check later if this needs a copy of original track

    // data settings
    smhdData.balance = XXH_swap16((uint16_t) (balance * (float)(1 << 8)));
    smhdData.reserved = 0;

    fileWrite->write((char*) &smhdData, sizeof(smhdData));
}

const std::string MP4::smhd::key = "smhd";

