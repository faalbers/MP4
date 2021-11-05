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
    mvhd(atomParse &parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    static std::string  key;
    uint32_t            timeScale;      // time units per second
    uint32_t            duration;       // amount of timeScale units
    float               preferredRate;
    float               preferredVolume;
    float               matrix[3][3];
    uint32_t            nextTrackID;
};

}
#endif