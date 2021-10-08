#include "url_.hpp"
#include <iostream>

MP4::url_::url_(atomBuild &build)
    : atom(build)
    , dataInSameFile(false)
{
    auto fileStream = build.getFileStream();

    versionBlock drefType;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &drefType, sizeof(drefType));
    if ( drefType.flag[2] == 1) dataInSameFile = true;
    if ( !dataInSameFile )
        throw std::runtime_error("url_ data reference should be in same file");
}

void MP4::url_::printData(bool fullLists)
{
    std::cout << "'url ' ) ["<< headerSize_ << "]";
    if (dataInSameFile)
        std::cout << " ( Data is in same file )";
    std::cout << std::endl;
}

void MP4::url_::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::url_::key = "url ";

