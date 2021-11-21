#ifndef MP4_ATOMWRITEFILE_H
#define MP4_ATOMWRITEFILE_H

#include <string>
#include <fstream>
#include <map>
#include <vector>

namespace MP4
{

class atomWriteFile
{
public:
    atomWriteFile(std::string fileName);
    ~atomWriteFile();

    std::string     getFilePath() const;
    std::ofstream*  getFileWrite();

    std::map<uint32_t, std::vector<uint64_t>> mdatWriteInfo;

private:
    void            error_(std::string message) const;

    std::string     filePath_;
    std::ofstream   fileStream_;

};

}

#endif