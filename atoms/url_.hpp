#ifndef MP4_URL__H
#define MP4_URL__H
#include <string>
#include <memory>
#include "atom.hpp"

/*
DATA REFERENCE TYPE URL_ ATOM:
Data reference is a Macintosh alias.
An alias contains information about the file it refers to, including its full path name.
Data not handled correctly yet.
*/

namespace MP4
{

class url_ : public atom
{
public:
    url_(atomParse& parse);

    void printData(bool fullLists = false);

    std::string getKey() const;

    static const std::string    key;
    bool                        dataInSameFile;
};

}
#endif