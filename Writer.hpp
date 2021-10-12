#ifndef MP4_WRITER_H
#define MP4_WRITER_H

#include "types.hpp"
#include "Processor.hpp"
#include <string>

namespace MP4
{

class Writer
{
public:
    Writer(Parser &parser);
    Writer(Processor &processor);

    std::string write(std::string fileName);
    
private:
    void    error_(std::string message);

    std::map<uint32_t, std::shared_ptr<trackType>>  tracks_;
    uint32_t                                        timeScale_;
    uint32_t                                        duration_;
};

}

#endif
