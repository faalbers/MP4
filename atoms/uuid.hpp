#ifndef MP4_UUID_H
#define MP4_UUID_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
*/

namespace MP4
{

class uuid : public atom
{
public:
    uuid(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    std::string         name;
    std::vector<std::shared_ptr<atom>>  uuidChildren;

};

}
#endif