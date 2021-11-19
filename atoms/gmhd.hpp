#ifndef MP4_GMHD_H
#define MP4_GMHD_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
BASE MEDIA INFORMAYION HEADER ATOM:
The base media information header atom indicates that this media information atom pertains to a base media.
*/

namespace MP4
{

class gmhd : public atom
{
public:
    gmhd(atomParse& parse);

    void printData(bool fullLists = false);

    std::string getKey();

    static std::string  key;
};

}
#endif