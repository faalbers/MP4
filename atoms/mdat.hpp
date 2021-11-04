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
    mdat(atomParse &parse);
    mdat(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    void writeHeader(std::ofstream &fileWrite);

    static std::string  key;
    int64_t             sampleDataPos;
    int64_t             sampleDataSize;

};

}
#endif