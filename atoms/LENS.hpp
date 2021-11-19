#ifndef MP4_LENS_H
#define MP4_LENS_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
USER DATA LENS ATOM :
User data with keyword 'LENS'. This is a GoPro user data atom.
We will just copy the data as I have no reference to the content
*/

namespace MP4
{

class LENS : public atom
{
public:
    LENS(atomParse& parse);
    LENS(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);

    std::string getKey() const;
    void getUserData(std::map<std::string, std::string>& userData_);

    void writeData(std::shared_ptr<atomWriteFile> writeFile);

    static const std::string    key;
    std::string                 userData;
    std::string                 lens;

};

}
#endif