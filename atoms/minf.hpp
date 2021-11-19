#ifndef MP4_MINF_H
#define MP4_MINF_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
MEDIA INFORMATION ATOM:
Media information atoms store handler-specific information for a track’s media data.
The media handler uses this information to map from media time to media data and to process the media data.
These atoms contain information that is specific to the type of data defined by the media.
Further, the format and content of media information atoms are dictated by the media handler that is
responsible for interpreting the media data stream.
Another media handler would not know how to interpret this information.
This atom contains the atoms like
    - that store media information for the video ('vmhd')
    - that store media information for the sound ('smhd')
    - that store media information for base ('gmhd') portions of QuickTime movies.
*/

namespace MP4
{

class minf : public atom
{
public:
    minf(atomParse& parse);
    minf(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey() const;

    static const std::string    key;
};

}
#endif