#include "GUMI.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::GUMI::GUMI(atomParse& parse)
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

MP4::GUMI::GUMI(std::shared_ptr<atomBuild> build)
    : atom(build)
    , userData(build->getUserData("GUMI"))
{
}

void MP4::GUMI::printData(bool fullLists) const
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (GUMI User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "userDataSize: " << userData.size() << std::endl;
    //dataStringViz(userData, dataIndent);
}

std::string MP4::GUMI::getKey() const
{
    return key;
}

void MP4::GUMI::getUserData(std::map<std::string, std::string>& userData_) const
{
    userData_["GUMI"] = userData;
}

void MP4::GUMI::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    
    if ( userData.size() > 0 )
        fileWrite->write((char*) userData.c_str(), (size_t) userData.size());
}

const std::string MP4::GUMI::key = "GUMI";

