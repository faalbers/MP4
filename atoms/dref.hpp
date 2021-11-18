#ifndef MP4_DREF_H
#define MP4_DREF_H
#include <string>
#include <memory>
#include <map>
#include "atom.hpp"

/*
DATA REFERENCE ATOM
Data reference atoms contain tabular data that instructs the data handler component how to
access the media’s data.
*/

namespace MP4
{

class dref : public atom
{
public:
    dref(atomParse& parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);
    
    std::string getKey();

    static std::string                  key;
    std::map<uint32_t, std::shared_ptr<atom>>   dataReferences;
};

}
#endif