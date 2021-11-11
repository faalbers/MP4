#include "atomWriteFile.hpp"

#include <filesystem>
#include <iostream>

MP4::atomWriteFile::atomWriteFile(std::string fileName)
{
    filePath_ = std::filesystem::absolute(std::filesystem::path(fileName)).string();
    fileStream_ = std::ofstream(filePath_, std::ios::binary);
    if ( fileStream_.fail() ) error_("Construct can not write MP4 file: "+filePath_);
}

MP4::atomWriteFile::~atomWriteFile()
{
    fileStream_.close();
}

std::string MP4::atomWriteFile::getFilePath()
{
    return filePath_;
}

std::ofstream *MP4::atomWriteFile::getFileWrite()
{
    return &fileStream_;
}

void MP4::atomWriteFile::error_(std::string message)
{
    std::cout << "atomWriteFile: "<< std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
