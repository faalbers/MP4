#ifndef MP4_STBL_H
#define MP4_STBL_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
SAMPLE TABLE ATOM:
The sample table atom contains information for converting from media time to sample number to sample location.
This atom also indicates how to interpret the sample (for example, whether to decompress the video data and,
if so, how).
This section describes the format and content of the sample table atom.
It can contain the following:
    - sample description atom ('stsd')
    - the time-to-sample atom ('stts')
    - the sync sample atom ('stss')
    - the sample-to-chunk atom ('stsc')
    - the sample size atom ('stsz')
    - the chunk offset atom ('stco')
    - the shadow sync atom ('stsh')
Recent additions to the list of atom types that a sample table atom can contain are the optional
sample group description and sample-to-group atoms included in:
    - Appendix G: Audio Priming - Handling Encoder Delay in AAC.
The sample table atom contains all the time and data indexing of the media samples in a track.
Using tables, it is possible to locate samples in time, determine their type, and determine their size,
container, and offset into that container.
If the track that contains the sample table atom references no data, then the sample table atom does
not need to contain any child atoms (not a very useful media track).
If the track that the sample table atom is contained in does reference data, then the following child
atoms are required:
    - sample description
    - sample size
    - sample to chunk
    - chunk offset
All of the subtables of the sample table use the same total sample count.
The sample description atom must contain at least one entry.
A sample description atom is required because it contains the data reference index field that indicates
which data reference atom to use to retrieve the media samples.
Without the sample description, it is not possible to determine where the media samples are stored.
The sync sample atom is optional. If the sync sample atom is not present, all samples are implicitly
sync samples.
*/

namespace MP4
{

class stbl : public atom
{
public:
    stbl(atomParse& parse);
    stbl(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();

    static const std::string    key;
};

}
#endif