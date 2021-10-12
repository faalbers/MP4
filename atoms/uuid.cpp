#include "uuid.hpp"
#include <iostream>

MP4::uuid::uuid(atomParse &parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    fileStream->seekg(fileDataPos_, fileStream->beg);
    char nameBlock[4];
    fileStream->read((char *) nameBlock, 4);

    name = std::string(nameBlock).substr(0,4);

}

void MP4::uuid::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "name :" << name << std::endl;
}

void MP4::uuid::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::uuid::key = "uuid";

