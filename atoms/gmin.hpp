#ifndef MP4_GMIN_H
#define MP4_GMIN_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
BASE MEDIA INFO ATOM:
The base media info atom, contained in the base media information header atom ('gmhd'),
defines the media’s control information, including graphics mode and balance information.
*/

namespace MP4
{

class gmin : public atom
{
public:
    gmin(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    uint16_t            graphicMode;
    uint16_t            opColorR;
    uint16_t            opColorG;
    uint16_t            opColorB;
    uint16_t            balance;
};

}
#endif