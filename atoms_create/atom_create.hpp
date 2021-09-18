#ifndef MP4_ATOM_CREATE_H
#define MP4_ATOM_CREATE_H

#include <string>
#include "../datatypes.hpp"

namespace MP4
{

class atom_create
{
public:
    atom_create();
    atom_create(std::string key_, std::ofstream *fileWrite);

    std::string key;

protected:
    void createHeader_(bool size64 = false);
    void setSize_();

    std::ofstream *fileWrite_;
    int64_t filePos_, fileAtomSizePos_, fileDataPos_, fileNextPos_;
    bool size64_;
};

}

#endif