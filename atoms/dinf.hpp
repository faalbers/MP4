#ifndef MP4_DINF_H
#define MP4_DINF_H
#include <string>
#include <memory>
#include "atom.hpp"

/*
DATA INFORMATION ATOM:
The handler reference atom (described in Handler Reference Atoms) contains information specifying
the data handler component that provides access to the media data.
The data handler component uses the data information atom to interpret the media’s data.
*/

namespace MP4
{

class dinf : public atom
{
public:
    dinf(atomParse& parse);
    dinf(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();

    static const std::string    key;
};

}
#endif