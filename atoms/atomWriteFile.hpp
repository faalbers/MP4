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

    std::string             getFilePath() const;
    std::ofstream*          getFileWrite();
    void                    addTrakChunkOffset(uint32_t trackID, uint64_t chunkOffset);
    std::vector<uint64_t>   getTrakChunkOffsets(uint32_t trackID) const;

private:
    void            error_(std::string message) const;

    std::string     filePath_;
    std::ofstream   fileStream_;
    // data created while writing mdat, needed for later chunk offsets
    std::map<uint32_t, std::vector<uint64_t>> trackChunkOffsets_;

};

}

#endif