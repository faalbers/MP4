#include "tkhd.hpp"
#include <iostream>

MP4::tkhd::tkhd(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
    // data blocks for file reading
    typedef struct tkhdDataBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
        uint32_t    creationTime;
        uint32_t    modificationTime;
        uint32_t    trackID;
        uint32_t    reservedA;
        uint32_t    duration;           // the sum of the durations of all of the track’s edits.
                                        // if there is no edit list, then the duration is
                                        // the sum of the sample durations, converted into the
                                        // movie timescale
        uint8_t     reservedB[8];
        uint16_t    layer;
        uint16_t    alternateGroup;
        uint16_t    volume;             // fixed point
        uint16_t    reservedC;
        uint32_t    matrix[3][3];
        uint32_t    trackWidth;
        uint32_t    trackHeight;
    } tkhdDataBlock;

    // get data
    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("tkhd atom can not parse file: "+filePath);
    tkhdDataBlock tkhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &tkhdData, sizeof(tkhdData));
    
    trackID = _byteswap_ulong(tkhdData.trackID);
    duration = _byteswap_ulong(tkhdData.duration);
    tkhdData.volume = _byteswap_ushort(tkhdData.volume);
    volume = (float)tkhdData.volume / (float)(1 << 8);
    tkhdData.trackWidth = _byteswap_ulong(tkhdData.trackWidth);
    trackWidth = (float)tkhdData.trackWidth / (float)(1 << 16);
    tkhdData.trackHeight = _byteswap_ulong(tkhdData.trackHeight);
    trackHeight = (float)tkhdData.trackHeight / (float)(1 << 16);

    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 2; j++) {
            tkhdData.matrix[i][j] = _byteswap_ulong(tkhdData.matrix[i][j]);
            matrix[i][j] = (float)tkhdData.matrix[i][j] / (float)(1 << 16);
        }
        tkhdData.matrix[i][2] = _byteswap_ulong(tkhdData.matrix[i][2]);
        matrix[i][2] = (float)tkhdData.matrix[i][2] / (float)(1 << 30);
    }
    
    fileStream.close();
}

void MP4::tkhd::printData(int level)
{
    if ( level == -1 ) level = 0;
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string pathIndent = std::string(level*5, ' ');
    std::string dataIndent = std::string(level*5+(levelCount-1)*5+1, ' ');
    std::cout << pathIndent << path_ << " (Track Header Atom)" << std::endl;
    std::cout << dataIndent << "trackID    : " << trackID << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
    std::cout << dataIndent << "volume     : " << volume << std::endl;
    std::cout << dataIndent << "trackWidth : " << trackWidth << std::endl;
    std::cout << dataIndent << "trackHeight: " << trackHeight << std::endl;
}

std::string MP4::tkhd::key = "tkhd";

