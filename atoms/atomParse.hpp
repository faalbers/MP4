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

    std::string     getFilePath() const;
    int64_t         getFileSize() const;
    std::ifstream*  getFileStream();

    std::string parentPath;

private:
    std::shared_ptr<atomReadFile> readFile_;
};

}

#endif