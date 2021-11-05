#include "tkhd.hpp"
#include <iostream>

MP4::tkhd::tkhd(atomParse &parse)
    : atom(parse)
{
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

MP4::tkhd::tkhd(std::shared_ptr<atomBuild> build)
    : atom(build)
    , trackID(build->currentTrackID())
    , duration(build->getDuration())
{
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

void MP4::tkhd::writeData(std::ofstream &fileWrite)
{
    dataBlock tkhdData;
    tkhdData.trackID = XXH_swap32(trackID);
    tkhdData.duration = XXH_swap32(duration);
    //memcpy(&ftypData.majorBrand, majorBrand.c_str(), 4);
    //ftypData.version = XXH_swap32(version);
    fileWrite.write((char *) &tkhdData, sizeof(tkhdData));
    /*
    for ( auto brand : compatibleBrands ) {
        fileWrite.write((char *) brand.c_str(), 4);
    }
    */
}

std::string MP4::tkhd::key = "tkhd";

