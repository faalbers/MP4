#ifndef MP4_MDAT_H
#define MP4_MDAT_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
MEDIA DATA ATOM:
the media data atom contains the actual media samples
*/

namespace MP4
{

class mdat : public atom
{
public:
    mdat(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    int64_t             sampleDataPos;
    int64_t             sampleDataSize;

private:
    void timeReshuffle_(std::ofstream &fileWrite, char *data);
    void extract_(std::ofstream &fileWrite, char *data);
};

}
#endif