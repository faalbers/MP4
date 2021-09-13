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
    fileSize_ = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);
    fileStream.close();

    if ( fileSize_ < 8 ) throw std::runtime_error("MP4::MP4 can not open file: " + atomBuild.filePath);

    do {
        atomBuild.parentPath = "/";
        auto child = atom::makeAtom_(atomBuild);
        atomBuild.filePos = child->fileNextPos_;
        children_.push_back(child);
    } while ( atomBuild.filePos < fileSize_ );
}

void MP4::root::printData(bool fullLists)
{
    /*
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (User Data Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
    */
    std::cout << "MP4 file : " << filePath_ << std::endl;
    std::cout << "file size: " << fileSize_ << std::endl << std::endl;
}

void MP4::root::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::root::key = "root";

