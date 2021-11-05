#ifndef MP4_PROCESSOR_H
#define MP4_PROCESSOR_H

#include "types.hpp"
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

private:
    friend class    Writer;
    void            error_(std::string message);

    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    uint32_t                                        timeScale_;
    uint32_t                                        duration_;
};

}

#endif
