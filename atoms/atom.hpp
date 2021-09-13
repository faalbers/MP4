#ifndef MP4_ATOM_H
#define MP4_ATOM_H

#include "../datatypes.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <tuple>

namespace MP4
{

class atom
{
public:
    atom(internal::atomBuildType &atomBuild);
    
    template<typename T>
    std::vector<T *>     getTypeAtoms()
    {
        std::vector<T *> foundTypeAtoms;
        std::vector<std::shared_ptr<atom>> found;
        if ( T::key == key ) foundTypeAtoms.push_back((T *) this);
        getChildAtoms_(T::key, found);
        for( auto foundAtom : found ) foundTypeAtoms.push_back((T *) foundAtom.get());
        return foundTypeAtoms;
    }

    void printHierarchy(int pathWith = 35, int valLevel = 10);
    virtual void printData(bool fullLists = false);
    virtual void printHierarchyData(bool fullLists = false);

    virtual void                        write(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    virtual std::tuple<int64_t, bool>   writeHeader(std::ofstream &fileWrite);
    virtual void                        writeData(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    virtual void                        writeChildren(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);

    std::string     key;

protected:
    friend class MP4;
    friend class dref;
    friend class uuid;

    static std::shared_ptr<atom>    makeAtom_(internal::atomBuildType &atomBuild);
    static bool                     isContainer_(std::ifstream &fileStream, int64_t dataSize);
    void                            getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found);
    int                             nestLevel_(int level);
    void                            write_(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    std::tuple<int64_t, bool>       writeHeader_(std::ofstream &fileWrite);
    void                            writeData_(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    void                            writeChildren_(std::ofstream &fileWrite, internal::writeInfoType &writeInfo);
    void                            writeTail_(std::ofstream &fileWrite, int64_t writeSizePos, bool posVal64bit);
    
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