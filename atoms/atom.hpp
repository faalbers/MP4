#ifndef MP4_ATOM_H
#define MP4_ATOM_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <tuple>
#include "atomParse.hpp"
#include "atomBuild.hpp"
#include "atomWriteFile.hpp"
#include "atomCopyFile.hpp"

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

#define OFFSET_1904_1970 2082844800

namespace MP4
{

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
        if ( T::key == key_ ) foundTypeAtoms.push_back((T *) this);
        getChildAtoms_(T::key, found);
        for( auto foundAtom : found ) foundTypeAtoms.push_back((T *) foundAtom.get());
        return foundTypeAtoms;
    }

    void printHierarchy(int pathWith = 35, int valLevel = 10);
    static std::string getTimeString(uint32_t time, uint32_t timeScale);
    static std::string getDateTimeString(uint32_t seconds);
    static uint32_t getCurrentDateTime();
    static uint32_t timeScaleDuration(uint32_t duration, uint32_t sourceTimeScale, uint32_t targetTimeScale);

    virtual std::string getKey();
    virtual void getUserData(std::map<std::string, std::string> &userData);

    virtual void printData(bool fullLists = false);
    virtual void printHierarchyData(bool fullLists = false);

    virtual void write(std::shared_ptr<atomWriteFile> writeFile);
    virtual void writeHeader(std::shared_ptr<atomWriteFile> writeFile);
    virtual void writeData(std::shared_ptr<atomWriteFile> writeFile);
    virtual void writeChildren(std::shared_ptr<atomWriteFile> writeFile);
    virtual void writeTail(std::shared_ptr<atomWriteFile> writeFile);

    virtual void copy(std::shared_ptr<atomCopyFile> copyFile);
    virtual void copyHeader(std::shared_ptr<atomCopyFile> copyFile);
    virtual void copyData(std::shared_ptr<atomCopyFile> copyFile);
    virtual void copyChildren(std::shared_ptr<atomCopyFile> copyFile);
    virtual void copyTail(std::shared_ptr<atomCopyFile> copyFile);

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

    void                            error_(std::string message);

    static std::shared_ptr<atom>    makeAtom_(atomParse &parse);
    void                            setMoov_(moov *moveAtom);
    void                            setTrak_(trak *trakAtom);
    static bool                     isContainer_(std::ifstream *fileStream, int64_t dataSize);
    void                            getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found);
    int                             nestLevel_(int level);
    
    void                            write_(std::shared_ptr<atomWriteFile> writeFile);
    void                            writeHeader_(std::shared_ptr<atomWriteFile> writeFile);
    void                            writeData_(std::shared_ptr<atomWriteFile> writeFile);
    void                            writeChildren_(std::shared_ptr<atomWriteFile> writeFile);
    void                            writeTail_(std::shared_ptr<atomWriteFile> writeFile);
    
    void                            copy_(std::shared_ptr<atomCopyFile> copyFile);
    void                            copyHeader_(std::shared_ptr<atomCopyFile> copyFile);
    void                            copyData_(std::shared_ptr<atomCopyFile> copyFile);
    void                            copyChildren_(std::shared_ptr<atomCopyFile> copyFile);
    void                            copyTail_(std::shared_ptr<atomCopyFile> copyFile);

    std::string                         key_;
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
    int64_t                             writeHeaderSizePos_;
    bool                                writeHeaderSize64_;
    std::shared_ptr<atomBuild>          build_;
};

}

#endif