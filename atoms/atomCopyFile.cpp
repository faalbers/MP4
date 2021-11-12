#include "atomCopyFile.hpp"

#include <filesystem>
#include <iostream>

MP4::atomCopyFile::atomCopyFile(std::string fileName)
{
    filePath_ = std::filesystem::absolute(std::filesystem::path(fileName)).string();
    fileStream_ = std::ofstream(filePath_, std::ios::binary);
    if ( fileStream_.fail() ) error_("Construct can not write MP4 file: "+filePath_);
}

MP4::atomCopyFile::~atomCopyFile()
{
    fileStream_.close();
}

std::string MP4::atomCopyFile::getFilePath()
{
    return filePath_;
}

std::ofstream *MP4::atomCopyFile::getFileWrite()
{
    return &fileStream_;
}

void MP4::atomCopyFile::addExcludeAtom(std::string atomPath)
{
    excludeAtoms_.insert(atomPath);
}

void MP4::atomCopyFile::addFullCopyAtom(std::string atomPath)
{
    excludeAtoms_.insert(atomPath);
}

void MP4::atomCopyFile::addExcludeTrack(uint32_t trackID)
{
    excludeTracks_.insert(trackID);
}

bool MP4::atomCopyFile::isTrackExcluded(uint32_t trackID)
{
    return (excludeTracks_.find(trackID) != excludeTracks_.end());
}

bool MP4::atomCopyFile::isAtomExcluded(std::string atomPath)
{
    return (excludeAtoms_.find(atomPath) != excludeAtoms_.end());
}

void MP4::atomCopyFile::error_(std::string message)
{
    std::cout << "atomCopyFile: "<< std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
