#ifndef MP4_FTYP_CREATE_H
#define MP4_FTYP_CREATE_H
#include "atom_create.hpp"

namespace MP4
{

class ftyp_create : public atom_create
{
public:
    ftyp_create(splunkType &splunk);

};

}
#endif