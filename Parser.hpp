#ifndef MP4_PARSER_H
#define MP4_PARSER_H

#include "types.hpp"
#include <string>
#include "atoms/root.hpp"

namespace MP4
{

class Parser
{
public:
    Parser(std::string fileName);

    void    printHierarchy();
    void    printHierarchyData(bool fullLists = false);

private:
    std::string                         filePath_;
    int64_t                             fileSize_;
    std::shared_ptr<root>               rootAtom_;
};

}

#endif
