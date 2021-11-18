#ifndef MP4_MUID_H
#define MP4_MUID_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA MUID ATOM :
User data with keyword 'MUID'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class MUID : public atom
{
public:
    MUID(atomParse& parse);
    MUID(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();
    void getUserData(std::map<std::string, std::string>& userData_);

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static std::string          key;
    std::string                 userData;

};

}
#endif