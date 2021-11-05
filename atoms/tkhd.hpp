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
    tkhd(atomParse &parse);
    tkhd(std::shared_ptr<atomBuild> build);
    
    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    void writeData(std::ofstream &fileWrite);

    static std::string  key;
    uint32_t            creationTime;
    uint32_t            modificationTime;
    uint32_t            trackID;
    uint32_t            duration;
    uint16_t            layer;
    float               volume;
    float               trackWidth;
    float               trackHeight;
    float               matrix[3][3];

private:
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
};

}
#endif