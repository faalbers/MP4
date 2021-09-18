#include "atom_create.hpp"
#include <cstring>
#include <iostream>

MP4::atom_create::atom_create()
    : key("")
    , fileWrite_(nullptr)
    , filePos_(0)
    , fileAtomSizePos_(0)
    , fileDataPos_(0)
    , fileNextPos_(0)
    , size64_(false)
{
}

MP4::atom_create::atom_create(std::string key_, std::ofstream *fileWrite)
    : key(key_)
    , fileWrite_(fileWrite)
    , filePos_(0)
    , fileAtomSizePos_(0)
    , fileDataPos_(0)
    , fileNextPos_(0)
    , size64_(false)
{
}

void MP4::atom_create::createHeader_(bool size64)
{
    if ( fileWrite_ == nullptr )
        throw std::runtime_error("MP4::stom_create has no file write strezm");
    
    // create atom header
    filePos_ = fileWrite_->tellp();
    size64_ = size64;
    fileAtomSizePos_ = filePos_;
    std::string testKey = "blob";
    datablock::atomHeaderBlock  atomHeader;
    size_t headerSize;
    if ( size64_ ) {
        headerSize = sizeof(atomHeader);
        atomHeader.size32 = _byteswap_ulong((uint32_t) 1);
        fileAtomSizePos_ += 8;
    } else {
        headerSize = 8;
    }
    memcpy(&atomHeader.key, key.c_str(), 4);
    fileWrite_->write((char *) &atomHeader, headerSize);
    fileDataPos_ = fileWrite_->tellp();
}

void MP4::atom_create::setSize_()
{
    // update final atom size
    fileNextPos_ = fileWrite_->tellp();
    auto atomSize = fileNextPos_ - filePos_;
    if ( size64_ ) {
        uint64_t size64 = _byteswap_uint64(((uint64_t) atomSize));
        fileWrite_->seekp(fileAtomSizePos_, fileWrite_->beg);
        fileWrite_->write((char *) &size64, sizeof(size64));
    } else {
        uint32_t size32 = _byteswap_ulong(((uint32_t) atomSize));
        fileWrite_->seekp(fileAtomSizePos_, fileWrite_->beg);
        fileWrite_->write((char *) &size32, sizeof(size32));
    }
    fileWrite_->seekp(fileNextPos_, fileWrite_->beg);
}
