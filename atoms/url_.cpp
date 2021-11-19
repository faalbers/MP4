#include "url_.hpp"
#include <iostream>

MP4::url_::url_(atomParse& parse)
    : atom(parse)
    , dataInSameFile(false)
{
    auto fileStream = parse.getFileStream();

    versionBlock drefType;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &drefType, sizeof(drefType));
    if ( drefType.flag[2] == 1) dataInSameFile = true;
    if ( !dataInSameFile )
        throw std::runtime_error("url_ data reference should be in same file");
}

void MP4::url_::printData(bool fullLists) const
{
    std::cout << "'url ' ) ["<< headerSize_ << "]";
    if (dataInSameFile)
        std::cout << " ( Data is in same file )";
    std::cout << std::endl;
}

std::string MP4::url_::getKey() const
{
    return key;
}

const std::string MP4::url_::key = "url ";

