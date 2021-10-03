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
    mdhd(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    uint32_t            timeScale;  // time units per second
    uint32_t            duration;   // amount of timeScale units
    uint32_t            creationTime;
    uint32_t            modificationTime;
    uint16_t            language;
    uint16_t            quality;
};

}
#endif

