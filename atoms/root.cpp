#include "root.hpp"
#include "trak.hpp"
#include "moov.hpp"
#include "ftyp.hpp"
#include "mdat.hpp"

#include <memory>

#include <iostream>

MP4::root::root(atomParse &parse)
{
    auto fileStream = parse.getFileStream();
    filePath_ = parse.getFilePath();
    size_ = parse.getFileSize();
    fileNextPos_ = size_;
    dataSize_ = size_;

    int64_t childNextPos;
    do {
        parse.parentPath = "/";
        auto child = atom::makeAtom_(parse);
        childNextPos = child->fileNextPos_;
        fileStream->seekg(childNextPos, fileStream->beg);
        children_.push_back(child);
    } while ( childNextPos < fileNextPos_ );

    // inject moov atom into all atoms
    for ( auto moov : getTypeAtoms<moov>() )
        for ( auto child : children_ ) child->setMoov_(moov);
    
    for ( auto trak : getTypeAtoms<trak>() ) trak->setTrak_(trak);
}

MP4::root::root(std::shared_ptr<atomBuild> build)
{
    build->parentPath = "/";
    std::shared_ptr<atom> child;
    
    child = std::make_shared<ftyp>(build);
    children_.push_back(child);
    
    child = std::make_shared<mdat>(build);
    children_.push_back(child);
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

void MP4::root::write(std::ofstream &fileWrite)
{
    std::cout << "BLAH\n" ;
    for ( auto child : children_ ) {
        std::cout << child->getKey() << std::endl;
        if ( child->getKey() == "ftyp" )
            child->write(fileWrite);
    }
    std::cout << "BLOH\n" ;
    //children_[0]->write(fileWrite);
    //children_[0]->write(fileWrite);
    //for ( auto child : children_ ) child->write(fileWrite);
}

std::string MP4::root::key = "root";

