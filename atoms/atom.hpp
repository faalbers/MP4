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
    
    void printHierarchy(int pathWith = 35, int valLevel = 10);
    virtual void printData();
    virtual void printHierarchyData();

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