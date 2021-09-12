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
    hdlr(internal::atomBuildType &atomBuild, std::string filePath, uint64_t filePos, std::string pathParent = "/");

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string      key;
    std::string             componentType;
    std::string             componentSubType;
    std::string             componentName;

};

}
#endif