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
    uuid(atomParse &parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    static std::string  key;
    std::string         name;
    std::vector<std::shared_ptr<atom>>  uuidChildren;

};

}
#endif