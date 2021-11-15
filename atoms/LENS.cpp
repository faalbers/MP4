#include "LENS.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::LENS::LENS(atomParse &parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    auto bufferSize = (size_t) dataSize_;
    auto buffer = new char[bufferSize];
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) buffer, bufferSize);
    userData = std::string(buffer,bufferSize);
    delete[] buffer;
    lens = getZeroTerminatedString(userData, bufferSize);
}

MP4::LENS::LENS(std::shared_ptr<atomBuild> build)
    : atom(build)
    , userData(build->getUserData("LENS"))
{
}

void MP4::LENS::printData(bool fullLists)
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (LENS User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "lens: " << lens << std::endl;
    //dataStringViz(userData, dataIndent);
}

void MP4::LENS::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::LENS::getKey()
{
    return key;
}

void MP4::LENS::getUserData(std::map<std::string, std::string> &userData_)
{
    userData_["LENS"] = userData;
}

void MP4::LENS::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    
    if ( userData.size() > 0 )
        fileWrite->write((char *) userData.c_str(), (size_t) userData.size());
}

std::string MP4::LENS::key = "LENS";

