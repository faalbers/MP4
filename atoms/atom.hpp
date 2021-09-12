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
    atom(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos, std::string pathParent = "/");
    
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

/*
    virtual void writeToFile(std::ofstream &fileWrite, char *data = nullptr);
    virtual void writeAtomDataToFile(std::ofstream &fileWrite, char *data);
    virtual void writeAtomChildrenToFile(std::ofstream &fileWrite, char *data);

    virtual void append(atom *appendAtom, std::ofstream &fileWrite, char *data = nullptr);
    virtual void appendData(atom *appendAtom, std::ofstream &fileWrite, char *data);
    virtual void appendChildren(atom *appendAtom, std::ofstream &fileWrite, char *data);
*/
    std::string     key;

protected:
    friend class MP4;
    friend class dref;
    friend class uuid;

    static std::shared_ptr<atom>    makeAtom_(internal::atomBuildType &atomBuild, std::string filePath, int64_t nextFilePos, std::string pathParent = "/");
    static bool                     isContainer_(std::ifstream &fileStream, int64_t dataSize);
    void                            getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found);
    int                             nestLevel_(int level);
    void                            writeAtomToFile_(std::ofstream &fileWrite, char *data);
    std::tuple<int64_t, bool>       writeAtomHeaderToFile_(std::ofstream &fileWrite);
    void                            writeAtomTailToFile_(std::ofstream &fileWrite, int64_t writeSizePos, bool posVal64bit);
    void                            writeAtomDataToFile_(std::ofstream &fileWrite, char *data);
    void                            writeAtomChildrenToFile_(std::ofstream &fileWrite, char *data);
    void                            append_(atom *appendAtom, std::ofstream &fileWrite, char *data);
    void                            appendData_(atom *appendAtom, std::ofstream &fileWrite, char *data);
    void                            appendChildren_(atom *appendAtom, std::ofstream &fileWrite, char *data);
    
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