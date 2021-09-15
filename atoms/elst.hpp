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
    elst(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    void appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo);

    static std::string  key;
    std::vector<elstEntryType> elstTable;

};

}
#endif