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
    typedef struct entryDataBlock
    {
        uint32_t    trackDuration;
        uint32_t    mediaStartTime;
        uint32_t    mediaRate;
    } entryDataBlock;

    typedef struct entryType
    {
        uint32_t    trackDuration;
        uint32_t    mediaStartTime;
        float       mediaRate;
    } entryType;

public:
    elst(atomParse& parse);

    void printData(bool fullLists = false) const;

    std::string getKey() const;

    static const std::string    key;
    std::vector<entryType> elstTable;

private:
    
};

}
#endif