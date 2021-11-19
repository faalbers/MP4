#ifndef MP4_BCID_H
#define MP4_BCID_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA BCID ATOM :
User data with keyword 'BCID'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class BCID : public atom
{
public:
    BCID(atomParse& parse);
    BCID(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;
    void getUserData(std::map<std::string, std::string>& userData_);

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string    key;
    std::string                 userData;

};

}
#endif