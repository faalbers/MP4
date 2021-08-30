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
    trak(std::string filePath, uint64_t filePos, std::string pathParent = "/");

    // data retrieval

    uint32_t                    getID();
    std::vector<std::string>    getSampleDataFormats();
    std::string                         getSampleDataReference(std::string dataFormat);
    // sample = (ID, duration, time, timeOffset) all time in track time units
    std::vector<sampleType>     getSamples();
    sampleType                  getSampleAtTime(float sampleTime); // sampleTime in seconds float
    uint32_t                    getChunksSize();
    // chunk  = (ID, sampleOffset)
    MP4::chunkOffsetType        sampleToChunk(sampleType sample);
    // chunk  = (ID, samples, firstSampleID, sampleDescriptionID)
    std::vector<chunkType>      getChunks();
    std::vector<uint64_t>       getChunkOffsets();

    // sample = (ID, duration, time, timeOffset) all time in track time units

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