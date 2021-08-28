#ifndef MP4_DREFENTRY_H
#define MP4_DREFENTRY_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
*/

namespace MP4
{

class drefEntry : public atom
{
public:
    drefEntry(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    static std::string  key;
    std::string                 reference;
};

}
#endif