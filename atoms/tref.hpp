#ifndef MP4_TREF_H
#define MP4_TREF_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
TRACK REFERENCE ATOM:
Track reference atoms define relationships between tracks.
Track reference atoms allow one track to specify how it is related to other tracks.
For example, if a movie has three video tracks and three sound tracks, track references allow you to
identify the related sound and video tracks.
Track references are unidirectional and point from the recipient track to the source track.
For example, a video track may reference a time code track to indicate where its time code is stored,
but the time code track would not reference the video track.
The time code track is the source of time information for the video track.
A single track may reference multiple tracks.
For example, a video track could reference a sound track to indicate that the two are synchronized and a
time code track to indicate where its time code is stored.
A single track may also be referenced by multiple tracks. For example, both a sound and video track could
reference the same time code track if they share the same timing information.
If this atom is not present, the track is not referencing any other track in any way.
Note that the array of track reference type atoms is sized to fill the track reference atom.
Track references with a reference index of 0 are permitted. This indicates no reference.
*/

namespace MP4
{

class tref : public atom
{
public:
    tref(atomParse& parse);
    tref(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;

    static const std::string    key;
};

}
#endif