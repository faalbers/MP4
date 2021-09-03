#include "MP4.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#ifdef MP4_PARSE_TIME
#include <chrono>
#endif

MP4::MP4::MP4(std::string fileName)
{
    #ifdef MP4_PARSE_TIME
    auto start = std::chrono::high_resolution_clock::now();
    #endif
    
    filePath = std::filesystem::absolute(std::filesystem::path(fileName)).string();

    std::ifstream fileStream(filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("MP4 can not open file: " + filePath);

    // get file length
    fileStream.seekg(0, fileStream.end);
    fileSize = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);
    fileStream.close();

    uint64_t nextFilePos = 0;
    do {
        auto child = atom::makeAtom_(filePath, nextFilePos);
        if ( child != nullptr ) {
            nextFilePos = child->fileNextPos_;
            children.push_back(child);
        }
    } while ( nextFilePos < fileSize );
    
    #ifdef MP4_PARSE_TIME
    auto end = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nMP4 Time : " << ms_int.count() << "ms\n";
    #endif
}

std::vector<MP4::trak *>  MP4::MP4::getTracks()
{
    for ( auto moov : getTypeAtoms<moov>()) return moov->getTypeAtoms<trak>();
    std::vector<trak *> foundTracks;
    return foundTracks;
}

int MP4::MP4::nestLevel()
{
    int level, maxLevel = 0;
    for ( auto child : children ) {
        level = child->nestLevel_(0);
        if ( level > maxLevel ) maxLevel = level;
    }
    return maxLevel;
}

void MP4::MP4::printHierarchy()
{
    int pathWith = nestLevel() * 5;
    int valWith = 10;
    std::cout << std::string(pathWith, '-') << std::string(valWith*5 + 28, '-') << std::endl;
    std::cout << std::setw(pathWith) << std::left << "<path>";
    std::cout <<"  |- " << std::setw(valWith) << std::right << "<start>"
        << " -|- " << std::setw(valWith) << "<data>"
        << " -|- " << std::setw(valWith) << "<end>"
        << " -| ( " << std::setw(valWith) << "<size>"
        << " ) ( " << std::setw(valWith) << "<datasize>"
        << " )";
    std::cout << std::endl << std::string(pathWith, '-') << std::string(valWith*5 + 28, '-') << std::endl;

    for ( auto child : children ) child->printHierarchy(pathWith, valWith);
    
    std::cout << std::endl;
}

void MP4::MP4::printHierarchyData(bool fullLists)
{
    std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    for ( auto child : children ) child->printHierarchyData(fullLists);
}
