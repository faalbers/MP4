#ifndef MP4_ELST_H
#define MP4_ELST_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
EDIT LIST ATOM
You use the edit list atom to map from a time in a movie to a time in a media, and ultimately to media data.
This information is in the form of entries in an edit list table
*/

namespace MP4
{

class elst : public atom
{
public:
    elst(atomParse &parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
    typedef struct entryType
    {
        uint32_t    duration;
        uint32_t    mediaTime;
        float       mediaRate;
    } entryType;
    std::vector<entryType> elstTable;

};

}
#endif