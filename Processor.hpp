#ifndef MP4_PROCESSOR_H
#define MP4_PROCESSOR_H

#include "types.hpp"
#include "Parser.hpp"
#include <map>
#include <memory>

namespace MP4
{

class Processor
{
public:
    Processor();

    void    addTrack(Parser &parser, uint32_t trackID);

private:
    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    uint32_t                                        timeScale_;
    uint32_t                                        duration_;
};

}

#endif
