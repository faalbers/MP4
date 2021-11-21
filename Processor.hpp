#ifndef MP4_PROCESSOR_H
#define MP4_PROCESSOR_H

#include "atoms/atomBuild.hpp"
#include "Parser.hpp"
#include <string>
#include <map>
#include <memory>

namespace MP4
{

class Processor
{
public:
    friend class atomBuild;
    Processor();

    void    addTrack(Parser &parser, uint32_t sourceTrackID, uint32_t targetTrackID = 0);
    void    addTrack(Parser &parser, std::string dataFormat, uint32_t targetTrackID = 0);
    void    addUserData(Parser &parser, std::string userDataKey = "");
    void    flattenTrackDurations();
    void    append(Parser &parser);
    void    test();

private:
    friend class    Writer;
    void            error_(std::string message) const;

    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    std::map<std::string, std::string>              userData_;
    uint32_t                                        videoTimeScale_;
    uint32_t                                        videoDuration_;
    uint32_t                                        creationTime_;
    uint32_t                                        modificationTime_;
    uint32_t                                        nextTrackID_;
};

}

#endif
