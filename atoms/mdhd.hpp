#ifndef MP4_MDHD_H
#define MP4_MDHD_H
#include <string>
#include <vector>
#include <memory>
#include "atom.hpp"

/*
MEDIA HEADER ATOM
The media header atom specifies the characteristics of a media, including time scale and duration
*/

namespace MP4
{

class mdhd : public atom
{
public:
    mdhd(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    uint32_t            timeScale;  // time units per second
    uint32_t            duration;   // amount of timeScale units

};

}
#endif