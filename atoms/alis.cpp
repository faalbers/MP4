#include "alis.hpp"
#include <iostream>

MP4::alis::alis(atomParse& parse)
    : atom(parse)
    , dataInSameFile(false)
{
    auto fileStream = parse.getFileStream();

    versionBlock drefType;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &drefType, sizeof(drefType));
    if ( drefType.flag[2] == 1) dataInSameFile = true;
    if ( !dataInSameFile )
        throw std::runtime_error("alis data reference should be in same file");
}

void MP4::alis::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference type 'alis' Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "dataInSameFile: " << dataInSameFile << std::endl;
}

std::string MP4::alis::getKey() const
{
    return key;
}

const std::string MP4::alis::key = "alis";

