#ifndef MP4_GPMD_H
#define MP4_GPMD_H
#include <string>
#include <vector>
#include <memory>
#include "atom.hpp"

/*
GOPRO METADATA ATOM
Gopro metadata mediainformation. It includes the version.
*/

namespace MP4
{

class gpmd : public atom
{
public:
    gpmd(atomBuild &build);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    uint32_t            version;

};

}
#endif