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
        for ( auto child : children )
            for ( auto typeAtom : child->getTypeAtoms<T>() )
                 foundTypeAtoms.push_back(typeAtom);
        return foundTypeAtoms;
    }
    std::vector<trak *> getTracks();
    trak                *getTrackFromID(uint32_t ID);
    void    printHierarchy();                           // Hierarchy print out of all atoms in file
    void    printHierarchyData(bool fullLists = false);  // Data print out. Only data that is handled so far
    void    mdatMap();
    void    mdatRemap();

    std::string                         filePath;
    int64_t                             fileSize;

private:
    int                                 nestLevel();

    std::vector<std::shared_ptr<atom>>  children;

};

}

#endif
