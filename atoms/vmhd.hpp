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
    vmhd(atomParse& parse);
    vmhd(std::shared_ptr<atomBuild> build);


    void printData(bool fullLists = false);

    std::string getKey() const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string    key;
    uint16_t                    graphicsMode;
    uint16_t                    opColorR;
    uint16_t                    opColorG;
    uint16_t                    opColorB;

private:
    typedef struct dataBlock
    {
        versionBlock    version;
        uint16_t        graphicsMode;
        uint16_t        opColorR;
        uint16_t        opColorG;
        uint16_t        opColorB;
    } dataBlock;
};

}
#endif