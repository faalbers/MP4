#include "atom.hpp"
#include "root.hpp"
#include "ftyp.hpp"
#include "uuid.hpp"
#include "udta.hpp"
#include "free.hpp"
#include "mdat.hpp"
#include "moov.hpp"
#include "mvhd.hpp"
#include "trak.hpp"
#include "tkhd.hpp"
#include "tref.hpp"
#include "tmcd.hpp"
#include "edts.hpp"
#include "elst.hpp"
#include "mdia.hpp"
#include "mdhd.hpp"
#include "hdlr.hpp"
#include "minf.hpp"
#include "gmhd.hpp"
#include "gmin.hpp"
#include "tcmi.hpp"
#include "vmhd.hpp"
#include "smhd.hpp"
#include "gpmd.hpp"
#include "dinf.hpp"
#include "dref.hpp"
#include "alis.hpp"
#include "url_.hpp"
#include "stbl.hpp"
#include "stsd.hpp"
#include "stts.hpp"
#include "stsc.hpp"
#include "stsz.hpp"
#include "stco.hpp"
#include "co64.hpp"
#include "stss.hpp"

#include <iostream>
#include <filesystem>

#include <iomanip>
#include "../Processor.hpp"

MP4::atomReadFile::atomReadFile(std::string fileName)
{
    filePath_ = std::filesystem::absolute(std::filesystem::path(fileName)).string();
    fileStream_ = std::ifstream(filePath_, std::ios::binary);
    if ( fileStream_.fail() )
        throw std::runtime_error("atomReadFile: can not find mp4 file: "+filePath_);
    fileStream_.seekg(0, fileStream_.end);
    fileSize_ = fileStream_.tellg();
    if ( fileSize_ < 8 )
        throw std::runtime_error("atomReadFile: mp4 file has no proper data: "+filePath_);
    fileStream_.seekg(0, fileStream_.beg);
}

MP4::atomReadFile::~atomReadFile()
{
    fileStream_.close();
}

std::string MP4::atomReadFile::getFilePath()
{
    return filePath_;
}

int64_t MP4::atomReadFile::getFileSize()
{
    return fileSize_;
}

std::ifstream *MP4::atomReadFile::getFileStream()
{
    return &fileStream_;
}

MP4::atomParse::atomParse(std::string fileName)
    : parentPath("/")
{
    readFile_ = std::make_shared<atomReadFile>(fileName);
}

std::string MP4::atomParse::getFilePath()
{
    if ( readFile_ != nullptr ) return readFile_->getFilePath();
    return "";
}

int64_t MP4::atomParse::getFileSize()
{
    if ( readFile_ != nullptr ) return readFile_->getFileSize();
    return 0;
}

std::ifstream *MP4::atomParse::getFileStream()
{
    if ( readFile_ != nullptr ) return readFile_->getFileStream();
    return nullptr;
}

MP4::atomBuild::atomBuild(Processor &processor)
    : timeScale_(processor.timeScale_)
    , duration_(processor.duration_)
    , tracks_(processor.tracks_)
{
}

MP4::atom::atom()
    : key("atom")
    , filePath_("")
    , path_("")
    , parentPath_("")
    , filePos_(0)
    , fileDataPos_(0)
    , fileNextPos_(0)
    , dataSize_(0)
    , moovAtom_(nullptr)
    , trakAtom_(nullptr)
    , build_(nullptr)
{
}

MP4::atom::atom(atomParse &parse)
    : filePath_(parse.getFilePath())
    , parentPath_(parse.parentPath)
    , moovAtom_(nullptr)
    , trakAtom_(nullptr)
    , build_(nullptr)
{
    auto fileStream = parse.getFileStream();
    int64_t fileSize;
    bool container;

    filePos_ = fileStream->tellg();

    // get file size and set file position to start of atom
    fileStream->seekg(0, fileStream->end);
    fileSize = fileStream->tellg();
    fileStream->seekg(filePos_, fileStream->beg);

    // read the header
    headerBlock dataBlock;
    fileStream->read((char *) &dataBlock, sizeof(dataBlock));

    key = std::string(dataBlock.key).substr(0,4);
    path_ = parentPath_ + key;

    // get atom size and data position
    dataBlock.size32 = XXH_swap32(dataBlock.size32);     // big to little endian
    dataBlock.size64 = XXH_swap64(dataBlock.size64);    // big to little endian
    if ( dataBlock.size32 == 1 ) {
        headerSize64_ = true;
        headerSize_ = 16;
        size_ = (int64_t) dataBlock.size64;
        fileDataPos_ = filePos_ + headerSize_;
    } else {
        headerSize64_ = false;
        headerSize_ = 8;
        size_ = (int64_t) dataBlock.size32;
        fileDataPos_ = filePos_ + headerSize_;
    }

    // set filestream to data position
    fileStream->seekg(fileDataPos_, fileStream->beg);

    // get other data
    fileNextPos_ = filePos_ + size_;
    dataSize_ = fileNextPos_ - fileDataPos_;
    container = isContainer_(fileStream, dataSize_);

    // find child atoms
    if ( !container ) return;
    int64_t childNextPos;
    do {
        parse.parentPath = path_+"/";
        auto child = makeAtom_(parse);
        childNextPos = child->fileNextPos_;
        fileStream->seekg(childNextPos, fileStream->beg);
        children_.push_back(child);
    } while ( childNextPos < fileNextPos_ );

}

MP4::atom::atom(std::shared_ptr<atomBuild> build)
    : parentPath_(build->parentPath)
    , filePath_("")
    , path_("")
    , filePos_(0)
    , fileDataPos_(0)
    , fileNextPos_(0)
    , dataSize_(0)
    , moovAtom_(nullptr)
    , trakAtom_(nullptr)
    , build_(build)
{
}

void MP4::atom::setMoov_(moov *moovAtom)
{
    moovAtom_ = moovAtom;
    for (auto child : children_ ) child->setMoov_(moovAtom);
}

void MP4::atom::setTrak_(trak *trakAtom)
{
    trakAtom_ = trakAtom;
    for (auto child : children_ ) child->setTrak_(trakAtom);
}

int MP4::atom::nestLevel_(int level)
{
    level++;
    int childLevel, maxChildLevel = level;
    for (auto child : children_ ) {
        childLevel = child->nestLevel_(level);
        if ( childLevel > maxChildLevel ) maxChildLevel = childLevel;
    }
    return maxChildLevel;
}

void MP4::atom::printHierarchy(int pathWith, int valLevel)
{
    std::cout << std::setw(pathWith) << std::left << path_;
    std::cout << "  |- " <<  std::setw(valLevel) << std::right << filePos_
        << " -|- " << std::setw(valLevel) << fileDataPos_
        << " -|- " << std::setw(valLevel) << fileNextPos_ << " -| "
        << "( " << std::setw(valLevel) << size_ << " ) ( " << std::setw(valLevel) << dataSize_ << " )";
    std::cout << std::endl;
    for ( auto child : children_ )
        child->printHierarchy(pathWith, valLevel);
}

void MP4::atom::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Atom) ["<< headerSize_ << "]" << std::endl;
    if ( dataSize_ == 0 )
        std::cout << dataIndent << "This Atom is empty ..." << std::endl;
    else
        std::cout << dataIndent << "No data defined ..." << std::endl;
}

void MP4::atom::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::shared_ptr<MP4::atom> MP4::atom::makeAtom_(atomParse &parse)
{
    std::shared_ptr<atom> newAtom;

    auto fileStream = parse.getFileStream();

    char charKey[4];
    auto filePos = fileStream->tellg();
    fileStream->seekg(4, fileStream->cur);
    fileStream->read((char *) &charKey, sizeof(charKey));
    fileStream->seekg(filePos, fileStream->beg);

    std::string key = std::string(charKey).substr(0,4);

    if ( key == "ftyp" ) newAtom = std::make_shared<ftyp>(parse);
    else if ( key == "uuid" ) newAtom = std::make_shared<uuid>(parse);
    else if ( key == "udta" ) newAtom = std::make_shared<udta>(parse);
    else if ( key == "free" ) newAtom = std::make_shared<free>(parse);
    else if ( key == "mdat" ) newAtom = std::make_shared<mdat>(parse);
    else if ( key == "moov" ) newAtom = std::make_shared<moov>(parse);
    else if ( key == "mvhd" ) newAtom = std::make_shared<mvhd>(parse);
    else if ( key == "trak" ) newAtom = std::make_shared<trak>(parse);
    else if ( key == "tkhd" ) newAtom = std::make_shared<tkhd>(parse);
    else if ( key == "tref" ) newAtom = std::make_shared<tref>(parse);
    else if ( key == "tmcd" ) newAtom = std::make_shared<tmcd>(parse);
    else if ( key == "edts" ) newAtom = std::make_shared<edts>(parse);
    else if ( key == "elst" ) newAtom = std::make_shared<elst>(parse);
    else if ( key == "mdia" ) newAtom = std::make_shared<mdia>(parse);
    else if ( key == "mdhd" ) newAtom = std::make_shared<mdhd>(parse);
    else if ( key == "hdlr" ) newAtom = std::make_shared<hdlr>(parse);
    else if ( key == "minf" ) newAtom = std::make_shared<minf>(parse);
    else if ( key == "gmhd" ) newAtom = std::make_shared<gmhd>(parse);
    else if ( key == "gmin" ) newAtom = std::make_shared<gmin>(parse);
    else if ( key == "tcmi" ) newAtom = std::make_shared<tcmi>(parse);
    else if ( key == "vmhd" ) newAtom = std::make_shared<vmhd>(parse);
    else if ( key == "smhd" ) newAtom = std::make_shared<smhd>(parse);
    else if ( key == "gpmd" ) newAtom = std::make_shared<gpmd>(parse);
    else if ( key == "dinf" ) newAtom = std::make_shared<dinf>(parse);
    else if ( key == "dref" ) newAtom = std::make_shared<dref>(parse);
    else if ( key == "alis" ) newAtom = std::make_shared<alis>(parse);
    else if ( key == "url " ) newAtom = std::make_shared<url_>(parse);
    else if ( key == "stbl" ) newAtom = std::make_shared<stbl>(parse);
    else if ( key == "stsd" ) newAtom = std::make_shared<stsd>(parse);
    else if ( key == "stts" ) newAtom = std::make_shared<stts>(parse);
    else if ( key == "stsc" ) newAtom = std::make_shared<stsc>(parse);
    else if ( key == "stsz" ) newAtom = std::make_shared<stsz>(parse);
    else if ( key == "stco" ) newAtom = std::make_shared<stco>(parse);
    else if ( key == "co64" ) newAtom = std::make_shared<co64>(parse);
    else if ( key == "stss" ) newAtom = std::make_shared<stss>(parse);
    else newAtom = std::make_shared<atom>(parse);

    return newAtom;
}

bool MP4::atom::isContainer_(std::ifstream *fileStream, int64_t dataSize)
{
    int64_t startPos = fileStream->tellg(), nextPos = startPos;
    fileStream->seekg(0, fileStream->end);
    int64_t fileSize = fileStream->tellg();
    fileStream->seekg(startPos, fileStream->beg);

    headerBlock dataBlock;
    int64_t size, totalSize = 0;
    bool result = false;
    do {
        fileStream->read((char *) &dataBlock, sizeof(dataBlock));
        dataBlock.size32 = XXH_swap32(dataBlock.size32);     // big to little endian
        dataBlock.size64 = XXH_swap64(dataBlock.size64);    // big to little endian
        if ( dataBlock.size32 == 1 ) size = dataBlock.size64;
        else size = (int64_t) dataBlock.size32;

        if ( size < 8 || size > (dataSize-totalSize) ) break;
        
        totalSize += size;
        if ( totalSize == dataSize ) {
            result = true;
            break;
        }

        nextPos += size;
        fileStream->seekg(nextPos, fileStream->beg);
    } while ( totalSize < dataSize );

    fileStream->seekg(startPos, fileStream->beg);
    return result;
}

void MP4::atom::getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found)
{
    for ( auto child : children_ ) {
        if ( child->key == findKey ) found.push_back(child);
        child->getChildAtoms_(findKey, found);
    }
}
