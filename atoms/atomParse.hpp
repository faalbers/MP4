#ifndef MP4_ATOMPARSE_H
#define MP4_ATOMPARSE_H

#include <string>
#include <memory>
#include "atomReadFile.hpp"

namespace MP4
{

class atomParse
{
public:
    atomParse(std::string fileName);

    std::string     getFilePath();
    int64_t         getFileSize();
    std::ifstream   *getFileStream();

    std::string parentPath;

private:
    std::shared_ptr<atomReadFile> readFile_;
};

}

#endif