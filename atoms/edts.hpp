#ifndef MP4_EDTS_H
#define MP4_EDTS_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
EDIT ATOM:
You use edit atoms to define the portions of the media that are to be used to build up a track for a movie.
The edits themselves are contained in an edit list table, which consists of time offset and duration values
for each segment.
*/

namespace MP4
{

class edts : public atom
{
public:
    edts(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;

};

}
#endif