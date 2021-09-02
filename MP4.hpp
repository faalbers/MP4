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

    std::vector<std::shared_ptr<atom>>  getAtoms(std::string findKey, atom *parent = nullptr);
    template<typename T>
    std::vector<T *>     getTypeAtoms(atom *parent = nullptr)
    {
        std::vector<T *> foundTypeAtoms;
        for( auto foundAtom : getAtoms(T::key, parent) ) foundTypeAtoms.push_back((T *) foundAtom.get());
        return foundTypeAtoms;
    }
    std::vector<trak *>  getTracks();
    void     printHierarchy();                           // Hierarchy print out of all atoms in file
    void     printHierarchyData(bool fullLists = false);  // Data print out. Only data that is handled so far

    std::string                         filePath;
    int64_t                             fileSize;

private:
    int                                 nestLevel();

    std::vector<std::shared_ptr<atom>>  children;

};

}

#endif
