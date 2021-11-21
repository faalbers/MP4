#include "atomReadFile.hpp"

#include <filesystem>
#include <iostream>

MP4::atomReadFile::atomReadFile(std::string fileName)
{
    filePath_ = std::filesystem::absolute(std::filesystem::path(fileName)).string();
    fileStream_ = std::ifstream(filePath_, std::ios::binary);
    if ( fileStream_.fail() ) error_("Construct can not read MP4 file: "+filePath_);
    fileStream_.seekg(0, fileStream_.end);
    fileSize_ = fileStream_.tellg();
    if ( fileSize_ < 8 )
        error_("Construct MP4 file has no proper data: "+filePath_);
    fileStream_.seekg(0, fileStream_.beg);
}

MP4::atomReadFile::~atomReadFile()
{
    fileStream_.close();
}

std::string MP4::atomReadFile::getFilePath() const
{
    return filePath_;
}

int64_t MP4::atomReadFile::getFileSize() const
{
    return fileSize_;
}

std::ifstream* MP4::atomReadFile::getFileStream()
{
    return &fileStream_;
}

void MP4::atomReadFile::error_(std::string message) const
{
    std::cout << "atomReadFile: "<< std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
