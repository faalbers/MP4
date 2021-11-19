#include "ftyp.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::ftyp::ftyp(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    fileStream->seekg(fileDataPos_, fileStream->beg);
    dataBlock ftypData;
    fileStream->read((char*) &ftypData, sizeof(ftypData));
    
    majorBrand = std::string(ftypData.majorBrand).substr(0,4);
    version = XXH_swap32(ftypData.version);
    
    std::stringstream date;
    date << std::hex << std::setfill('0')
    << std::setw(2) << (int) ((version >> 8) & 0xff) << "/"
    << std::setw(2) << (int) (version & 0xff) << "/"
    << std::setw(2) << (int) ((version >> 24) & 0xff)
    << std::setw(2) << (int) ((version >> 16) & 0xff);
    versionString = date.str();

    char type[4];
    auto types = (fileNextPos_ - fileStream->tellg()) / 4;
    while ( types > 0 ) {
        fileStream->read((char*) &type, sizeof(type));
        compatibleBrands.push_back(std::string(type).substr(0,4));;
        types--;
    }
}

MP4::ftyp::ftyp(std::shared_ptr<atomBuild> build)
    : atom(build)
    , majorBrand("mp41")
    , version(538120216)
    , versionString("10/18/2013")
{
    compatibleBrands.push_back("mp41");
}

void MP4::ftyp::printData(bool fullLists)
{    
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (File Type Atom) ["<< headerSize_ << "]" << std::endl;
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

std::string MP4::ftyp::getKey() const
{
    return key;
}

void MP4::ftyp::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock ftypData;
    memcpy(&ftypData.majorBrand, majorBrand.c_str(), 4);
    ftypData.version = XXH_swap32(version);
    fileWrite->write((char*) &ftypData, sizeof(ftypData));
    for ( auto brand : compatibleBrands ) {
        fileWrite->write((char*) brand.c_str(), 4);
    }
}

const std::string MP4::ftyp::key = "ftyp";

