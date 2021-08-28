#ifndef MP4_ATOM_H
#define MP4_ATOM_H

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <string>

namespace MP4
{

class atom
{
public:
    atom(std::string filePath, uint64_t filePos, std::string pathParent = "/");
    
    std::vector<std::shared_ptr<atom>>  getAtoms(std::string findKey, atom *parent = nullptr);
    template<typename T>
    std::vector<T *>     getTypeAtoms(atom *parent = nullptr)
    {
        std::vector<T *> foundTypeAtoms;
        for( auto foundAtom : getAtoms(T::key, parent) ) foundTypeAtoms.push_back((T *) foundAtom.get());
        return foundTypeAtoms;
    }

    void printHierarchy(int pathWith = 35, int valLevel = 10);
    virtual void printData(bool fullLists = false);
    virtual void printHierarchyData(bool fullLists = false);

    std::string                         key;

protected:
    friend class MP4;

    static std::shared_ptr<atom>    makeAtom_(std::string filePath, int64_t nextFilePos, std::string pathParent = "/");
    static bool                     isContainer_(std::ifstream &fileStream, int64_t dataSize);
    void                            getAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found);
    int                             nestLevel_(int level);

    int64_t                             size_;
    std::string                         filePath_;
    std::string                         path_;
    std::string                         parentPath_;
    int64_t                             filePos_, fileDataPos_, fileNextPos_;
    int64_t                             dataSize_;
    std::vector<std::shared_ptr<atom>>  children_;
};

}

#endif