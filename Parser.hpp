#ifndef MP4_PARSER_H
#define MP4_PARSER_H

#include "types.hpp"
#include <string>
#include <set>
#include <memory>
#include "atoms/root.hpp"

namespace MP4
{

class Parser
{
public:
    Parser(std::string fileName);

    void    printHierarchy();
    void    printHierarchyData(bool fullLists = false);

    std::set<std::string>   getDataFormats();
    std::set<std::string>   getComponentTypes();
    std::set<std::string>   getComponentSubTypes();
    std::set<uint32_t>      getDataFormatTrackIDs(std::string dataFormat);
    std::set<uint32_t>      getComponentSubTypeTrackIDs(std::string componentSubType);

    std::shared_ptr<trackType>  getTrack(uint32_t trackID);

private:
    void                    error_(std::string message);

    std::string             filePath_;
    int64_t                 fileSize_;
    std::shared_ptr<root>   rootAtom_;
};

}

#endif
