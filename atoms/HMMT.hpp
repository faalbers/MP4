#ifndef MP4_HMMT_H
#define MP4_HMMT_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA HMMT ATOM :
User data with keyword 'HMMT'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class HMMT : public atom
{
public:
    HMMT(atomParse& parse);
    HMMT(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;
    void getUserData(std::map<std::string, std::string>& userData_) const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string    key;
    std::string                 userData;

};

}
#endif