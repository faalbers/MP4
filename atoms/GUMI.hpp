#ifndef MP4_GUMI_H
#define MP4_GUMI_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA GUMI ATOM :
User data with keyword 'GUMI'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class GUMI : public atom
{
public:
    GUMI(atomParse& parse);
    GUMI(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;
    void getUserData(std::map<std::string, std::string>& userData_);

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string    key;
    std::string                 userData;

};

}
#endif