#ifndef MP4_ALIS_H
#define MP4_ALIS_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
DATA REFERENCE TYPE ALIS ATOM:
Data reference is a Macintosh alias.
An alias contains information about the file it refers to, including its full path name.
Data not handled correctly yet.
*/

namespace MP4
{

class alis : public atom
{
public:
    alis(atomParse& parse);
    alis(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile) const;

    static const std::string    key;
    bool                        dataInSameFile;
};

}
#endif