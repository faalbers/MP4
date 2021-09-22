#ifndef MP4_TMCD_H
#define MP4_TMCD_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

// this atom can have several uses as the use the same tmcd key name

/*
TIMECODE TRACK REFERENCE ATOM:
A track reference type atom for time code containing the track reference information.
Track references with a reference index of 0 are permitted. This indicates no reference.
*/

/*
TIMECODE SAMPLE DESCRIPTION ATOM:
This can also be a timecode sample description container atom under gmhd. The atoms below contain
information that defines how to interpret time code media data.
This sample description is based on the standard sample description header.
*/

namespace MP4
{

class tmcd : public atom
{
public:
    tmcd(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string      key;

    // data if it's a timecode track reference
    bool                    isTrackReference;
    std::vector<uint32_t>   trackIDs;
    // if it's not a track reference atom , it is a container
};

}
#endif