#ifndef MP4_DREF_H
#define MP4_DREF_H
#include <string>
#include <memory>
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
    dref(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);
    
    static std::string          key;
    std::vector<drefEntryType>  drefTable;

};

}
#endif