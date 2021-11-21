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

    void            setParentPath(std::string parentPath);
    std::string     getParentPath() const;
    std::string     getFilePath() const;
    int64_t         getFileSize() const;
    std::ifstream*  getFileStream();

private:
    std::string parentPath_;
    std::shared_ptr<atomReadFile> readFile_;
};

}

#endif