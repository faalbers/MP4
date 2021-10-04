#ifndef MP4_VMHD_H
#define MP4_VMHD_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
VIDEO MEDIA INFORMATION HEADER ATOM:
These atoms define specific color and graphics mode information.
*/

namespace MP4
{

class vmhd : public atom
{
public:
    vmhd(atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    uint16_t            graphicMode;
    uint16_t            opColorR;
    uint16_t            opColorG;
    uint16_t            opColorB;
};

}
#endif