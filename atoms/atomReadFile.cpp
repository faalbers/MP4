#include "atomReadFile.hpp"

#include <filesystem>

MP4::atomReadFile::atomReadFile(std::string fileName)
{
    filePath_ = std::filesystem::absolute(std::filesystem::path(fileName)).string();
    fileStream_ = std::ifstream(filePath_, std::ios::binary);
    if ( fileStream_.fail() )
        throw std::runtime_error("atomReadFile: can not find mp4 file: "+filePath_);
    fileStream_.seekg(0, fileStream_.end);
    fileSize_ = fileStream_.tellg();
    if ( fileSize_ < 8 )
        throw std::runtime_error("atomReadFile: mp4 file has no proper data: "+filePath_);
    fileStream_.seekg(0, fileStream_.beg);
}

MP4::atomReadFile::~atomReadFile()
{
    fileStream_.close();
}

std::string MP4::atomReadFile::getFilePath()
{
    return filePath_;
}

int64_t MP4::atomReadFile::getFileSize()
{
    return fileSize_;
}

std::ifstream *MP4::atomReadFile::getFileStream()
{
    return &fileStream_;
}
