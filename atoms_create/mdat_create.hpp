#ifndef MP4_MDAT_CREATE_H
#define MP4_MDAT_CREATE_H
#include "atom_create.hpp"

namespace MP4
{

class mdat_create : public atom_create
{
public:
    mdat_create(splunkType &splunk);

};

}
#endif