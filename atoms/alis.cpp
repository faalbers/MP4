#include "alis.hpp"
#include <iostream>

MP4::alis::alis(atomParse parse)
    : atom(parse)
    , dataInSameFile(false)
{
    auto fileStream = parse.getFileStream();

    versionBlock drefType;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &drefType, sizeof(drefType));
    if ( drefType.flag[2] == 1) dataInSameFile = true;
    if ( !dataInSameFile )
        throw std::runtime_error("alis data reference should be in same file");
}

void MP4::alis::printData(bool fullLists)
{
    std::cout << "'alis' )";
    if (dataInSameFile)
        std::cout << " ( Data is in same file )";
    std::cout << std::endl;
}

void MP4::alis::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::alis::key = "alis";

