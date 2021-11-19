#ifndef MP4_HDLR_H
#define MP4_HDLR_H
#include <string>
#include <vector>
#include <memory>
#include "atom.hpp"

/*
HANDLER REFERENCE ATOM:
The handler reference atom specifies the media handler component that is to be used to
interpret the media’s data.
Historically, the handler reference atom was also used for data references.
However, this use is no longer current and may now be safely ignored.
The handler atom within a media atom declares the process by which the media data in
the stream may be presented, and thus, the nature of the media in a stream.
For example, a video handler would handle a video track.
*/

namespace MP4
{

class hdlr : public atom
{
public:
    hdlr(atomParse& parse);
    hdlr(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string    key;
    std::string                 componentType;
    std::string                 componentSubType;
    std::string                 componentName;

private:
    typedef struct dataBlock
    {
        versionBlock    version;
        char            componentType[4];
        char            componentSubType[4];
        uint32_t        componentManufacturer;
        uint32_t        componentFlags;
        uint32_t        componentFlagsMask;
    } dataBlock;

};

}
#endif