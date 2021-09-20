#ifndef MP4_MP4_H
#define MP4_MP4_H

#include "datatypes.hpp"
#include "atoms.hpp"
#include <string>
#include <vector>
#include <memory>

namespace MP4
{

class MP4
{
public:
    MP4(std::string fileName);

    template<typename T>
    std::vector<T *>     getTypeAtoms()
    {
        std::vector<T *> foundTypeAtoms;
        for ( auto typeAtom : rootAtom_->getTypeAtoms<T>() )
            foundTypeAtoms.push_back(typeAtom);
        return foundTypeAtoms;
    }
    std::vector<trak *> getTracks();
    trak                *getTrackFromID(uint32_t ID);
    void    printHierarchy();                           // Hierarchy print out of all atoms in file
    void    printHierarchyData(bool fullLists = false);  // Data print out. Only data that is handled so far

    void    write(std::string filePath_, writeSettingsType &writeSettings);
    void    append(MP4 &appendMP4, std::string filePath_, writeSettingsType &writeSettings);

    splunkType getSplunk();
    splunkType appendSplunk(MP4 &appendMP4, std::string fileWritePath);
    void createFromSplunk(splunkType &splunk);
    void createFromSplunkOld(splunkType &splunk);

    std::string                         filePath;
    int64_t                             fileSize;

private:
    int                                 nestLevel();

    std::shared_ptr<root>               rootAtom_;

};

}

#endif
