#ifndef MP4_ATOMREADFILE_H
#define MP4_ATOMREADFILE_H

#include <string>
#include <fstream>

namespace MP4
{

class atomReadFile
{
public:
    atomReadFile(std::string fileName);
    ~atomReadFile();

    std::string     getFilePath();
    int64_t         getFileSize();
    std::ifstream   *getFileStream();

private:
    void            error_(std::string message);

    std::string     filePath_;
    std::ifstream   fileStream_;
    int64_t         fileSize_;

};

}

#endif