#ifndef MP4_FREE_H
#define MP4_FREE_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
FREE SPACE ATOM:
Unused free space in file.
*/

namespace MP4
{

class free : public atom
{
public:
    free(atomParse& parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    static std::string  key;
    int64_t             freeSize;
};

}
#endif