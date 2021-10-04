#ifndef MP4_TCMI_H
#define MP4_TCMI_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
TIMECODE MEDIA INFORMATION ATOM:
The timecode media also requires a media information atom.
This atom contains information governing how the timecode text is displayed.
This media information atom is stored in a base media information atom.
*/

namespace MP4
{

class tcmi : public atom
{
public:
    tcmi(atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    uint16_t            textFont;
    uint16_t            textFace;
    uint16_t            textSize;
    uint16_t            textColorR;
    uint16_t            textColorG;
    uint16_t            textColorB;
    uint16_t            backgroundColorR;
    uint16_t            backgroundColorG;
    uint16_t            backgroundColorB;
    std::string         fontName;
};

}
#endif