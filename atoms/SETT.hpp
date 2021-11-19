#ifndef MP4_SETT_H
#define MP4_SETT_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA SETT ATOM :
User data with keyword 'SETT'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class SETT : public atom
{
public:
    SETT(atomParse& parse);
    SETT(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey();
    void getUserData(std::map<std::string, std::string>& userData_);

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string          key;
    std::string                 userData;

};

}
#endif