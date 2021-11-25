#ifndef MP4_PARSER_H
#define MP4_PARSER_H

#include <string>
#include <set>
#include <memory>
#include "dataTypes.hpp"
#include "atoms/root.hpp"

namespace MP4
{

class Parser
{
public:
    Parser(std::string fileName);

    void                        printHierarchy() const;
    void                        printHierarchyData(bool fullLists = false) const;
    void                        printMdatMap();

    std::string                 getFilePath() const;
    std::set<std::string>       getDataFormats();
    std::set<std::string>       getComponentTypes();
    std::set<std::string>       getComponentSubTypes();
    std::set<uint32_t>          getDataFormatTrackIDs(std::string dataFormat);
    std::set<uint32_t>          getComponentSubTypeTrackIDs(std::string componentSubType);

    std::shared_ptr<trackType>  getTrack(uint32_t trackID);
    std::shared_ptr<trackType>  getTrack(std::string dataFormat);
    void                        getUserData(std::map<std::string, std::string> &userData);
    std::string                 getUserData(std::string userDataKey);
    std::shared_ptr<root>       getRootAtom();

private:
    void                        error_(std::string message) const;

    std::string                 filePath_;
    int64_t                     fileSize_;
    std::shared_ptr<root>       rootAtom_;
};

}

#endif
