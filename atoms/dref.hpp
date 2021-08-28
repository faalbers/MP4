#ifndef MP4_DREF_H
#define MP4_DREF_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
*/

namespace MP4
{

class dref : public atom
{
public:
    dref(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);
    std::vector<std::string>    drefTable;

    static std::string  key;
};

}
#endif