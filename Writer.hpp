#ifndef MP4_WRITER_H
#define MP4_WRITER_H

#include "dataTypes.hpp"
#include "Processor.hpp"
#include <string>
#include "atoms/root.hpp"

namespace MP4
{

class Writer
{
public:
    Writer(Parser &parser);
    Writer(Processor &processor);

    std::string write(std::string fileName) const;
    
private:
    void    error_(std::string message) const;

    std::shared_ptr<root>   rootAtom_;
    std::shared_ptr<root>   rootAtomParser_;
};

}

#endif
