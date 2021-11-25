#ifndef MP4_ELST_H
#define MP4_ELST_H
#include <string>
#include <memory>
#include "atom.hpp"
#include "../dataTypes.hpp"

/*
EDIT LIST ATOM
You use the edit list atom to map from a time in a movie to a time in a media, and ultimately to media data.
This information is in the form of entries in an edit list table
*/

namespace MP4
{

class elst : public atom
{
    typedef struct entryDataBlock
    {
        uint32_t    trackDuration;
        uint32_t    mediaStartTime;
        uint32_t    mediaRate;
    } entryDataBlock;

public:
    elst(atomParse& parse);

    void printData(bool fullLists = false) const;

    std::string getKey() const;

    static const std::string    key;
    std::map<uint32_t, editListEntryType> elstTable;

private:
    
};

}
#endif