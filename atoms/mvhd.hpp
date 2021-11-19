#ifndef MP4_MVHD_H
#define MP4_MVHD_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
MOVIE HEADER ATOM
You use the movie header atom to specify the characteristics of an entire movie.
The data contained in this atom defines characteristics of the entire movie, such as time scale
and duration.
*/

namespace MP4
{

class mvhd : public atom
{
public:
    mvhd(atomParse& parse);
    mvhd(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static std::string  key;
    uint32_t            creationTime;
    uint32_t            modificationTime;
    uint32_t            timeScale;      // time units per second
    uint32_t            duration;       // amount of timeScale units
    float               preferredRate;
    float               preferredVolume;
    std::vector<std::vector<float>> matrix;
    uint32_t            nextTrackID;

private:
    typedef struct dataBlock
    {
        versionBlock    version;
        uint32_t        creationTime;
        uint32_t        modificationTime;
        uint32_t        timeScale;          // time units per second
        uint32_t        duration;           // amount of timeScale units
        uint32_t        preferredRate;      // fixed point
        uint16_t        preferredVolume;    // fixed point
        uint8_t         reserved[10];
        uint32_t        matrix[3][3];
        uint32_t        previewTime;
        uint32_t        previewDuration;
        uint32_t        posterTime;
        uint32_t        selectionTime;
        uint32_t        selectionDuration;
        uint32_t        currentTime;
        uint32_t        nextTrackID;
    } dataBlock;

};

}
#endif