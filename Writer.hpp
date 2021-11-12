#ifndef MP4_WRITER_H
#define MP4_WRITER_H

#include "types.hpp"
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

    std::string write(std::string fileName);
    std::string copyTest(std::string fileName);
    
private:
    void    error_(std::string message);

    std::shared_ptr<root>   rootAtom_;
    std::shared_ptr<root>   rootAtomParser_;
};

}

#endif
