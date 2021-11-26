#ifndef MP4_SMHD_H
#define MP4_SMHD_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
SOUND MEDIA INFORMATION HEADER ATOM:
The sound media information header atom stores the sound media’s control information, such as balance.
*/

namespace MP4
{

class smhd : public atom
{
public:
    smhd(atomParse& parse);
    smhd(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile) const;

    static const std::string    key;
    float                       balance;

private:
    typedef struct dataBlock
    {
        versionBlock    version;
        uint16_t        balance;
        uint16_t        reserved;
    } dataBlock;
};

}
#endif