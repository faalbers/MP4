#include "HMMT.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::HMMT::HMMT(atomParse& parse)
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

MP4::HMMT::HMMT(std::shared_ptr<atomBuild> build)
    : atom(build)
    , userData(build->getUserData("HMMT"))
{
}

void MP4::HMMT::printData(bool fullLists) const
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (HMMT User Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "userDataSize: " << userData.size() << std::endl;
    std::cout << dataIndent << "seems to be all zeros ..." << std::endl;
    //dataStringViz(userData, dataIndent);
}

std::string MP4::HMMT::getKey() const
{
    return key;
}

void MP4::HMMT::getUserData(std::map<std::string, std::string>& userData_) const
{
    userData_["HMMT"] = userData;
}

void MP4::HMMT::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    
    if ( userData.size() > 0 )
        fileWrite->write((char*) userData.c_str(), (size_t) userData.size());
}

const std::string MP4::HMMT::key = "HMMT";

