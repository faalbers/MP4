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
    mdhd(atomParse& parse);
    mdhd(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static std::string  key;
    uint32_t            creationTime;
    uint32_t            modificationTime;
    uint32_t            timeScale;  // time units per second
    uint32_t            duration;   // amount of timeScale units
    uint16_t            language;
    uint16_t            quality;

private:
    typedef struct dataBlock
    {
        versionBlock    version;
        uint32_t        creationTime;
        uint32_t        modificationTime;
        uint32_t        timeScale;          // time units per second
        uint32_t        duration;           // amount of timeScale units
        uint16_t        language;
        uint16_t        quality;
    } dataBlock;

};

}
#endif

