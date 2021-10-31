#ifndef MP4_ATOM_H
#define MP4_ATOM_H

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <tuple>
#include "../types.hpp"

#if defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define XXH_swap16 OSSwapInt16
#define XXH_swap32 OSSwapInt32
#define XXH_swap64 OSSwapInt64
#elif defined(WIN32)
#include <stdlib.h>
#define XXH_swap16 _byteswap_ushort
#define XXH_swap32 _byteswap_ulong
#define XXH_swap64 _byteswap_uint64
#endif

namespace MP4
{

class atomReadFile
{
public:
    atomReadFile(std::string fileName);
    ~atomReadFile();

    std::string     getFilePath();
    int64_t         getFileSize();
    std::ifstream   *getFileStream();

private:
    std::string     filePath_;
    std::ifstream   fileStream_;
    int64_t         fileSize_;

};

class atomParse
{
public:
    atomParse(std::string fileName);

    std::string     getFilePath();
    int64_t         getFileSize();
    std::ifstream   *getFileStream();

    std::string parentPath;

private:
    std::shared_ptr<atomReadFile> readFile_;
};

class Processor;
class atomBuild
{
public:
    atomBuild(Processor &processor);

    std::string parentPath;

private:
    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    uint32_t                                        timeScale_;
    uint32_t                                        duration_;
};

// because they include atom as base. included in cpp
class moov;
class trak;

class atom
{
public:
    atom();
    atom(atomParse &parse);
    atom(std::shared_ptr<atomBuild> build);
    
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

    std::string     key;

protected:
    typedef struct headerBlock
    {
        uint32_t    size32; // (big endian) size of atom 32 bit
        char        key[4]; // (4 char) FourCC key of atom
        uint64_t    size64; // (big endian) size of atom 64 bit
    } headerBlock;

    typedef struct versionBlock
    {
        uint8_t     version;
        uint8_t     flag[3];
    } atomVersionBlock;

    typedef struct tableBlock
    {
        versionBlock    version;
        uint32_t        numberOfEntries;
    } tableBlock;

    friend class Parser;
    friend class root;
    friend class dref;
    friend class uuid;
    friend class tkhd;

    static std::shared_ptr<atom>    makeAtom_(atomParse &parse);
    void                            setMoov_(moov *moveAtom);
    void                            setTrak_(trak *trakAtom);
    static bool                     isContainer_(std::ifstream *fileStream, int64_t dataSize);
    void                            getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found);
    int                             nestLevel_(int level);
    
    bool                                headerSize64_;
    int64_t                             size_;
    int64_t                             headerSize_;
    std::string                         filePath_;
    std::string                         path_;
    std::string                         parentPath_;
    int64_t                             filePos_, fileDataPos_, fileNextPos_;
    int64_t                             dataSize_;
    std::vector<std::shared_ptr<atom>>  children_;
    moov                                *moovAtom_;
    trak                                *trakAtom_;
    int64_t                             createHeaderSizePos_;
    bool                                createHeaderSize64_;
    std::shared_ptr<atomBuild>          build_;
};

}

#endif