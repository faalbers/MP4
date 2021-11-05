#ifndef MP4_MOOV_H
#define MP4_MOOV_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
MOVIE ATOM:
You use movie atoms to specify the information that defines a movie—that is, the information
that allows your application to interpret the sample data that is stored elsewhere.
The movie atom usually contains a movie header atom, which defines the time scale and duration
information for the entire movie, as well as its display characteristics.
Existing movies may contain a movie profile atom, which summarizes the main features of the movie,
such as the necessary codecs and maximum bit rate.
In addition, the movie atom contains a track atom for each track in the movie.
It contains other types of atoms, including at least one of three possible atoms
    - the movie header atom ('mvhd')
    - the compressed movie atom ('cmov')
    - or a reference movie atom ('rmra').
An uncompressed movie atom can contain both a movie header atom and a reference movie atom,
but it must contain at least one of the two.
It can also contain several other atoms, such as
    - a clipping atom ('clip')
    - one or more track atoms ('trak')
    - a color table atom ('ctab')
    - and a user data atom ('udta').
Compressed movie atoms and reference movie atoms are discussed separately.
This section describes normal uncompressed movie atoms.
*/

namespace MP4
{

class moov : public atom
{
public:
    moov(atomParse &parse);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);
    
    std::string getKey();

    static std::string  key;
};

}
#endif