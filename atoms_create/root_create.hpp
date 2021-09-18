#ifndef MP4_ROOT_CREATE_H
#define MP4_ROOT_CREATE_H
#include "atom_create.hpp"

namespace MP4
{

class root_create : public atom_create
{
public:
    root_create(splunkType &splunk);
};

}
#endif