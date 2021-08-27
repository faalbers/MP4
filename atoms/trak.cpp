#include "trak.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::trak::trak(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
    , index(-1)
{
    for ( auto child : children_ ) {
        if ( child->key_ == "tkhd" ) {
            auto trakTkhd = (tkhd *) child.get();
            trackID = trakTkhd->trackID;
            duration = trakTkhd->duration;
            volume = trakTkhd->volume;
            trackWidth = trakTkhd->trackWidth;
            trackHeight = trakTkhd->trackHeight;
            //for ( int i = 0; i < 3; i++ )
            //    for ( int j = 0; j < 3; i++ ) matrix[i][j] = trakTkhd->matrix[i][j];
            return;
        }
    }
}

void MP4::trak::printData(int level)
{
    if ( level == -1 ) level = 0;
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string pathIndent = std::string(level*5, ' ');
    std::string dataIndent = std::string(level*5+(levelCount-1)*5+1, ' ');
    std::cout << pathIndent << path_ << " (Track Atom)" << std::endl;
    std::cout << dataIndent << "trackID    : " << trackID << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
    std::cout << dataIndent << "volume     : " << volume << std::endl;
    std::cout << dataIndent << "trackWidth : " << trackWidth << std::endl;
    std::cout << dataIndent << "trackHeight: " << trackHeight << std::endl;
    /*
    std::cout << dataIndent << "matrix:" << std::endl;
    for ( int i = 0; i < 3; i++ )
        std::cout << dataIndent << matrix[i][0] << " " << matrix[i][1] << " " << matrix[i][2] << " " << std::endl;
    */
}

std::string MP4::trak::key = "trak";

