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
    dref(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;
    
    std::string getKey() const;

    void write(std::shared_ptr<atomWriteFile> writeFile) const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile) const;

    bool    isDataInSameFile(uint16_t referenceIndex) const;
    std::map<uint16_t, dataReferenceEntryType> 
            getDataReferences() const;

    static const std::string    key;
    uint32_t                    dataReferenceCount;
};

}
#endif