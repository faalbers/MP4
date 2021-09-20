#include "root.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::root::root(internal::atomBuildType &atomBuild)
{
    filePath_ = atomBuild.filePath;
    atomBuild.filePos = 0;

    std::ifstream fileStream(atomBuild.filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("MP4::MP4 can not open file: " + atomBuild.filePath);

    // get file length
    fileStream.seekg(0, fileStream.end);
    size_ = fileStream.tellg();
    fileNextPos_ = size_;
    dataSize_ = size_;
    fileStream.seekg(0, fileStream.beg);
    fileStream.close();

    if ( size_ < 8 ) throw std::runtime_error("MP4::MP4 can not open file: " + atomBuild.filePath);

    do {
        atomBuild.parentPath = "/";
        auto child = atom::makeAtom_(atomBuild);
        atomBuild.filePos = child->fileNextPos_;
        children_.push_back(child);
    } while ( atomBuild.filePos < fileNextPos_ );

    // inject moov atom into all atoms
    for ( auto moov : getTypeAtoms<moov>() )
        for ( auto child : children_ ) child->setMoov_(moov);
    
    for ( auto trak : getTypeAtoms<trak>() ) trak->setTrak_(trak);
}

void MP4::root::printData(bool fullLists)
{
    std::cout << "MP4 file : " << filePath_ << std::endl;
    std::cout << "file size: " << size_ << std::endl << std::endl;
}

void MP4::root::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::root::create(splunkType &splunk)
{
    for ( auto child : children_ ) {
        if ( child->key == "mdat" ) child->create(splunk);
    }
}

std::string MP4::root::key = "root";

