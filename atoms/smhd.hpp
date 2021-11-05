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
    smhd(atomParse &parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    static std::string  key;
    float               balance;
};

}
#endif