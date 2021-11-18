#ifndef MP4_GPMF_H
#define MP4_GPMF_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA GPMF ATOM :
User data with keyword 'GPMF'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class GPMF : public atom
{
public:
    GPMF(atomParse& parse);
    GPMF(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false) const;
    void printHierarchyData(bool fullLists = false) const;

    std::string getKey();
    void getUserData(std::map<std::string, std::string>& userData_);

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static std::string          key;
    std::string                 userData;

};

}
#endif