#ifndef MP4_CAME_H
#define MP4_CAME_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA CAME ATOM :
User data with keyword 'CAME'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class CAME : public atom
{
public:
    CAME(atomParse& parse);
    CAME(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;

    std::string getKey() const;
    void getUserData(std::map<std::string, std::string>& userData_) const;

    void writeData(std::shared_ptr<atomWriteFile> writeFile) const;

    static const std::string    key;
    std::string                 userData;

};

}
#endif