#include "ftyp.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::ftyp::ftyp(internal::atomBuildType &atomBuild, uint64_t filePos)
    : atom(atomBuild, filePos)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mdhd atom can not parse file: "+filePath_);
    fileStream.seekg(fileDataPos_, fileStream.beg);
    datablock::ftypDataBlock ftypData;
    fileStream.read((char *) &ftypData, sizeof(ftypData));
    
    majorBrand = std::string(ftypData.majorBrand).substr(0,4);
    version = _byteswap_ulong(ftypData.version);
    
    std::stringstream date;
    date << std::hex << std::setfill('0')
    << std::setw(2) << (int) ((version >> 8) & 0xff) << "/"
    << std::setw(2) << (int) (version & 0xff) << "/"
    << std::setw(2) << (int) ((version >> 24) & 0xff)
    << std::setw(2) << (int) ((version >> 16) & 0xff);
    versionString = date.str();

    char type[4];
    auto types = (fileNextPos_ - fileStream.tellg()) / 4;
    while ( types > 0 ) {
        fileStream.read((char *) &type, sizeof(type));
        compatibleBrands.push_back(std::string(type).substr(0,4));;
        types--;
    }
    fileStream.close();
}

void MP4::ftyp::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (File Type Atom)" << std::endl;
    std::cout << dataIndent << "majorBrand    : " << majorBrand << std::endl;
    std::cout << dataIndent << "version       : " << version << std::endl;
    std::cout << dataIndent << "version string: " << versionString << std::endl;
    int index = 1;
    std::cout << dataIndent << "[#] (compatible brands)\n";
    for ( auto entry : compatibleBrands ) {
        std::cout << dataIndent << "[" << index << "] ( '" << entry << "' )" << std::endl;
        index++;
    }

}

void MP4::ftyp::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::ftyp::key = "ftyp";

