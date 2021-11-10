#ifndef MP4_ATOMWRITEFILE_H
#define MP4_ATOMWRITEFILE_H

#include <string>
#include <fstream>
#include "../types.hpp"

namespace MP4
{

class atomWriteFile
{
public:
    atomWriteFile(std::string fileName);
    ~atomWriteFile();

    std::string     getFilePath();
    std::ofstream   *getFileWrite();

private:
    void            error_(std::string message);

    std::string     filePath_;
    std::ofstream   fileStream_;

};

}

#endif