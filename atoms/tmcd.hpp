#ifndef MP4_TMCD_H
#define MP4_TMCD_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
TIME CODE TRACK REFERENCE ATOM
A track reference type atom for time code containing the track reference information.
Track references with a reference index of 0 are permitted. This indicates no reference.
*/

namespace MP4
{

class tmcd : public atom
{
public:
    tmcd(internal::atomBuildType &atomBuild);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    static std::string      key;
    std::vector<uint32_t>   trackIDs;
};

}
#endif