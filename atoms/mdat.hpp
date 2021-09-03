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
    mdat(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    int64_t             sampleDataPos;
    int64_t             sampleDataSize;
};

}
#endif