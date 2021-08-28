#ifndef MP4_TKHD_H
#define MP4_TKHD_H
#include <string>
#include <vector>
#include <memory>
#include "atom.hpp"

/*
TRACK HEADER ATOM
The track header atom specifies the characteristics of a single track within a movie.
*/
namespace MP4

{

class tkhd : public atom
{
public:
    tkhd(std::string filePath, uint64_t filePos, std::string pathParent = "/");
    
    void printData();
    void printHierarchyData();

    static std::string  key;
    uint32_t            trackID;
    uint32_t            duration;
    float               volume;
    float               trackWidth;
    float               trackHeight;
    float               matrix[3][3];

};

}
#endif