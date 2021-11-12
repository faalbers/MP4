#include "FIRM.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::FIRM::FIRM(atomParse &parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    auto bufferSize = (size_t) dataSize_;
    auto buffer = new char[bufferSize];
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) buffer, bufferSize);
    userData = std::string(buffer,bufferSize);
    delete[] buffer;

}

MP4::FIRM::FIRM(std::shared_ptr<atomBuild> build)
    : atom(build)
    , userData("")
{
}

void MP4::FIRM::printData(bool fullLists)
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (FIRM User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "userDataSize: " << userData.size() << std::endl;
}

void MP4::FIRM::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::FIRM::getKey()
{
    return key;
}

void MP4::FIRM::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    
    if ( userData.size() > 0 )
        fileWrite->write((char *) userData.c_str(), (size_t) userData.size());
}

std::string MP4::FIRM::key = "FIRM";

