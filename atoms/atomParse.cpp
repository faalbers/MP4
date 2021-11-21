#include "atomParse.hpp"

MP4::atomParse::atomParse(std::string fileName)
    : parentPath_("/")
{
    readFile_ = std::make_shared<atomReadFile>(fileName);
}

void MP4::atomParse::setParentPath(std::string parentPath)
{
    parentPath_ = parentPath;
}

std::string MP4::atomParse::getParentPath() const
{
    return parentPath_;
}

std::string MP4::atomParse::getFilePath() const
{
    if ( readFile_ != nullptr ) return readFile_->getFilePath();
    return "";
}

int64_t MP4::atomParse::getFileSize() const
{
    if ( readFile_ != nullptr ) return readFile_->getFileSize();
    return 0;
}

std::ifstream* MP4::atomParse::getFileStream()
{
    if ( readFile_ != nullptr ) return readFile_->getFileStream();
    return nullptr;
}
