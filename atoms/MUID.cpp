#include "MUID.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::MUID::MUID(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    auto bufferSize = (size_t) dataSize_;
    auto buffer = new char[bufferSize];
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) buffer, bufferSize);
    userData = std::string(buffer,bufferSize);
    delete[] buffer;

}

MP4::MUID::MUID(std::shared_ptr<atomBuild> build)
    : atom(build)
    , userData(build->getUserData("MUID"))
{
}

void MP4::MUID::printData(bool fullLists)
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (MUID User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "userDataSize: " << userData.size() << std::endl;
    //dataStringViz(userData, dataIndent);
}

void MP4::MUID::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::MUID::getKey()
{
    return key;
}

void MP4::MUID::getUserData(std::map<std::string, std::string>& userData_)
{
    userData_["MUID"] = userData;
}

void MP4::MUID::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    
    if ( userData.size() > 0 )
        fileWrite->write((char*) userData.c_str(), (size_t) userData.size());
}

std::string MP4::MUID::key = "MUID";

