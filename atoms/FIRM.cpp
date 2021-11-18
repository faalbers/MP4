#include "FIRM.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::FIRM::FIRM(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    auto bufferSize = (size_t) dataSize_;
    auto buffer = new char[bufferSize];
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) buffer, bufferSize);
    userData = std::string(buffer,bufferSize);
    delete[] buffer;
    firmware = getZeroTerminatedString(userData, bufferSize);
}

MP4::FIRM::FIRM(std::shared_ptr<atomBuild> build)
    : atom(build)
    , userData(build->getUserData("FIRM"))
{
}

void MP4::FIRM::printData(bool fullLists) const
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (FIRM User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "firmware: " << firmware << std::endl;
    //dataStringViz(userData, dataIndent);
}

void MP4::FIRM::printHierarchyData(bool fullLists) const
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::FIRM::getKey()
{
    return key;
}

void MP4::FIRM::getUserData(std::map<std::string, std::string>& userData_)
{
    userData_["FIRM"] = userData;
}

void MP4::FIRM::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    
    if ( userData.size() > 0 )
        fileWrite->write((char*) userData.c_str(), (size_t) userData.size());
}

std::string MP4::FIRM::key = "FIRM";

