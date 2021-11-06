#include "mdhd.hpp"
#include <iostream>

MP4::mdhd::mdhd(atomParse &parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    dataBlock mdhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &mdhdData, sizeof(mdhdData));
    creationTime = XXH_swap32(mdhdData.creationTime);
    modificationTime = XXH_swap32(mdhdData.modificationTime);
    timeScale = XXH_swap32(mdhdData.timeScale);
    duration = XXH_swap32(mdhdData.duration);
    language = XXH_swap16(mdhdData.language);
    quality = XXH_swap16(mdhdData.quality);
}

void MP4::mdhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "creationTime    : " << getDateTime(creationTime) << std::endl;
    std::cout << dataIndent << "modificationTime: " << getDateTime(modificationTime) << std::endl;
    std::cout << dataIndent << "timeScale       : " << timeScale << std::endl;
    std::cout << dataIndent << "duration        : " << duration << std::endl;
    std::cout << dataIndent << "language        : " << language << std::endl;
    std::cout << dataIndent << "quality         : " << quality << std::endl;
}

void MP4::mdhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::mdhd::getKey()
{
    return key;
}

std::string MP4::mdhd::key = "mdhd";

