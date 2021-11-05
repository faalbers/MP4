#include "tkhd.hpp"
#include <iostream>

MP4::tkhd::tkhd(atomParse &parse)
    : atom(parse)
{
typedef struct dataBlock
{
    versionBlock    version;
    uint32_t        creationTime;
    uint32_t        modificationTime;
    uint32_t        trackID;
    uint32_t        reservedA;
    uint32_t        duration;   // the sum of the durations of all of the track’s edits.
                                // if there is no edit list, then the duration is
                                // the sum of the sample durations, converted into the
                                // movie timescale
    uint8_t         reservedB[8];
    uint16_t        layer;
    uint16_t        alternateGroup;
    uint16_t        volume;             // fixed point
    uint16_t        reservedC;
    uint32_t        matrix[3][3];
    uint32_t        trackWidth;
    uint32_t        trackHeight;
} dataBlock;
    // get data
    auto fileStream = parse.getFileStream();

    dataBlock tkhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char *) &tkhdData, sizeof(tkhdData));
    
    trackID = XXH_swap32(tkhdData.trackID);
    duration = XXH_swap32(tkhdData.duration);
    tkhdData.volume = XXH_swap16(tkhdData.volume);
    volume = (float)tkhdData.volume / (float)(1 << 8);
    tkhdData.trackWidth = XXH_swap32(tkhdData.trackWidth);
    trackWidth = (float)tkhdData.trackWidth / (float)(1 << 16);
    tkhdData.trackHeight = XXH_swap32(tkhdData.trackHeight);
    trackHeight = (float)tkhdData.trackHeight / (float)(1 << 16);

    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 2; j++) {
            tkhdData.matrix[i][j] = XXH_swap32(tkhdData.matrix[i][j]);
            matrix[i][j] = (float)tkhdData.matrix[i][j] / (float)(1 << 16);
        }
        tkhdData.matrix[i][2] = XXH_swap32(tkhdData.matrix[i][2]);
        matrix[i][2] = (float)tkhdData.matrix[i][2] / (float)(1 << 30);
    }
    
}

void MP4::tkhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "trackID    : " << trackID << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
    std::cout << dataIndent << "volume     : " << volume << std::endl;
    std::cout << dataIndent << "trackWidth : " << trackWidth << std::endl;
    std::cout << dataIndent << "trackHeight: " << trackHeight << std::endl;
    std::cout << dataIndent << "matrix     :" << std::endl;
    std::cout << dataIndent << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << std::endl;
    std::cout << dataIndent << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << std::endl;
    std::cout << dataIndent << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << std::endl;
}

void MP4::tkhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::tkhd::getKey()
{
    return key;
}

std::string MP4::tkhd::key = "tkhd";

