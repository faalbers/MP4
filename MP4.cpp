#include "MP4.hpp"
#include <fstream>
#include <filesystem>

MP4::MP4::MP4(std::string fileName)
{
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
}

std::vector<std::shared_ptr<MP4::atom>> MP4::MP4::getAtoms(std::string findKey, atom *parent)
{
    std::vector<std::shared_ptr<atom>> found;
    if ( parent != nullptr ) parent->getAtoms_(findKey, found);
    else for ( auto child : children ) {
        if ( child->key == findKey ) found.push_back(child);
        child->getAtoms_(findKey, found);
    }
    return found;
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

void MP4::MP4::printHierarchyData()
{
    std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    for ( auto child : children ) child->printHierarchyData();
    /*
    for ( auto moov : getTypeAtoms<moov>() ) {
        moov->printData();
        std::cout << std::string(26, '-') << " TRACKS " << std::string(26, '-') << std::endl;
        for ( auto trak : getTypeAtoms<trak>(moov) ) {
            trak->printData();
            for ( auto mdia : getTypeAtoms<mdia>(trak) ) {
                mdia->printData();
                for ( auto hdlr : getTypeAtoms<hdlr>(mdia) ) hdlr->printData();
                for ( auto dref : getTypeAtoms<dref>(mdia) ) dref->printData();
                for ( auto stsd : getTypeAtoms<stsd>(mdia) ) stsd->printData();
                for ( auto stts : getTypeAtoms<stts>(mdia) ) stts->printData();
                for ( auto stsc : getTypeAtoms<stsc>(mdia) ) stsc->printData();
                for ( auto stsz : getTypeAtoms<stsz>(mdia) ) stsz->printData();
                for ( auto stco : getTypeAtoms<stco>(mdia) ) stco->printData();
                for ( auto co64 : getTypeAtoms<co64>(mdia) ) co64->printData();
                std::cout << std::string(60, '-') << std::endl;
            }
        }
    }
    */
}
