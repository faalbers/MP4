#ifndef MP4_TRAK_H
#define MP4_TRAK_H
#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include "atom.hpp"

/*
TRACK ATOM:
Track atoms define a single track of a movie. A movie may consist of one or more tracks.
Each track is independent of the other tracks in the movie and carries its own temporal
and spatial information. Each track atom contains its associated media atom.
Tracks are used specifically for the following purposes:
    - To contain media data references and descriptions (media tracks).
    - To contain modifier tracks (tweens, and so forth).
    - To contain packetization information for streaming protocols (hint tracks).
      Hint tracks may contain references to media sample data or copies of media sample data.
      For more information about hint tracks, refer to Hint Media.
*/

namespace MP4
{

class trak : public atom
{
public:
    trak(internal::atomBuildType &atomBuild);

    // data retrieval

    uint32_t                    getID();

    uint32_t                    getMediaTimeScale(); // float in seconds

    trackDataType               getTrackData();
    size_t                      getSampleCount();
    std::map<uint32_t, stsdEntryType>   getSampleDescriptions();

    std::map<uint32_t, chunkType>       getChunks();
    size_t                      getChunkCount();
    std::map<uint32_t, uint64_t>    getChunkOffsets();

    bool isDataInSameFile();

    // track checkers
    bool isComponentType(std::string type);
    bool isComponentSubType(std::string type);
    bool hasSampleDataFormat(std::string format);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string  key;
};

}
#endif