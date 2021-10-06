#ifndef MP4_PROCESSOR_H
#define MP4_PROCESSOR_H

#include "types.hpp"
#include <map>

namespace MP4
{

class Processor
{
public:
    Processor();

private:
    std::map<uint32_t, trackType>   tracks_;
    uint32_t                        timeScale_;
    uint32_t                        duration_;
};

}

#endif
