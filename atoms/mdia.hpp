#ifndef MP4_MDIA_H
#define MP4_MDIA_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
MEDIA ATOM:
Media atoms describe and define a track’s media type and sample data.
The media atom contains information that specifies:
    - The media type, such as sound, video or timed metadata
    - The media handler component used to interpret the sample data
    - The media timescale and track duration
    - Media-and-track-specific information, such as sound volume or graphics mode
    - The media data references, which typically specify the file where the sample data is stored
    - The sample table atoms, which, for each media sample, specify the sample description, duration,
      and byte offset from the data reference.
It must contain a media header atom ('mdhd'), and it can contain:
    - a handler reference ('hdlr') atom
    - a media information ('minf') atom
    - a user data ('udta') atom.
*/

namespace MP4
{

class mdia : public atom
{
public:
    mdia(atomParse& parse);
    mdia(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey() const;

    static const std::string    key;
};

}
#endif