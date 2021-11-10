#ifndef MP4_MDAT_H
#define MP4_MDAT_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
MEDIA DATA ATOM:
the media data atom contains the actual media samples
*/

namespace MP4
{

class mdat : public atom
{
public:
    mdat(atomParse &parse);
    mdat(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static std::string  key;
    int64_t             sampleDataPos;
    int64_t             sampleDataSize;

private:
    typedef struct mdatWriteType_
    {
        std::string filePath;
        uint64_t    filePos;
        size_t      size;
    } mdatWriteType_;

    std::vector<mdatWriteType_> mdatWrite_;
};

}
#endif