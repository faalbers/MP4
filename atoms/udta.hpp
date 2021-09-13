#ifndef MP4_UDTA_H
#define MP4_UDTA_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
USER DATA ATOM:
User data atoms allow you to define and store data associated with a QuickTime object, such as
a movie 'moov', track 'trak', or media 'mdia'. This includes both information that QuickTime looks for,
such as copyright information or whether a movie should loop, and arbitrary information—provided by and
for your application—that QuickTime simply ignores.
A user data atom whose immediate parent is a movie atom contains data relevant to the movie as a whole.
A user data atom whose parent is a track atom contains information relevant to that specific track.
A QuickTime movie file may contain many user data atoms, but only one user data atom is allowed as the
immediate child of any given movie atom or track atom.
Inside the user data atom is a list of atoms describing each piece of user data.
User data provides a simple way to extend the information stored in a QuickTime movie.
For example, user data atoms can store a movie’s window position, playback characteristics, or creation
information.
Applications should ignore any data atom types that they do not recognize.
*/

namespace MP4
{

class udta : public atom
{
public:
    udta(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
};

}
#endif