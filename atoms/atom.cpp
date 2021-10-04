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
#include <iomanip>

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
{

}

MP4::atom::atom(atomBuildType &atomBuild)
    : filePath_(atomBuild.filePath)
    , filePos_(atomBuild.filePos)
    , parentPath_(atomBuild.parentPath)
    , moovAtom_(nullptr)
    , trakAtom_(nullptr)
{
    int64_t fileSize;
    bool container;

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("Atom can not parse file: "+filePath_);

    // get file size anset file position to start of atom
    fileStream.seekg(0, fileStream.end);
    fileSize = fileStream.tellg();
    fileStream.seekg(filePos_, fileStream.beg);

    // read the header
    headerBlock dataBlock;
    fileStream.read((char *) &dataBlock, sizeof(dataBlock));

    key = std::string(dataBlock.key).substr(0,4);
    path_ = parentPath_ + key;
    #ifdef MP4_PARSE_PATH
    std::cout << path_ << std::endl;
    #endif

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
    fileStream.seekg(fileDataPos_, fileStream.beg);

    // get other data
    fileNextPos_ = filePos_ + size_;
    dataSize_ = fileNextPos_ - fileDataPos_;
    container = isContainer_(fileStream, dataSize_);
    fileStream.close();

    #ifdef MP4_PARSE_DATA
    std::cout << "  size       : " << size_ << std::endl;
    std::cout << "  filePos    : " << filePos_ << std::endl;
    std::cout << "  fileDataPos: " << fileDataPos_ << std::endl;
    std::cout << "  dataSize   : " << dataSize_ << std::endl;
    std::cout << "  fileNextPos: " << fileNextPos_ << std::endl;
    #endif

    // find child atoms
    if ( !container ) return;
    atomBuild.filePos = fileDataPos_;
    do {
        atomBuild.parentPath = path_+"/";
        auto child = makeAtom_(atomBuild);
        atomBuild.filePos = child->fileNextPos_;
        children_.push_back(child);
    } while ( atomBuild.filePos < fileNextPos_ );
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

std::shared_ptr<MP4::atom> MP4::atom::makeAtom_(atomBuildType &atomBuild)
{
    std::shared_ptr<atom> newAtom;

    std::ifstream fileStream(atomBuild.filePath, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("Atom can not open file: "+atomBuild.filePath);

    // get file size anset file position to start of atom
    fileStream.seekg(atomBuild.filePos, fileStream.beg);
    char    charKey[4];;
    fileStream.read((char *) &charKey, sizeof(charKey));
    fileStream.read((char *) &charKey, sizeof(charKey));
    fileStream.close();
    std::string key = std::string(charKey).substr(0,4);

    if ( key == "root" ) newAtom = std::make_shared<root>(atomBuild);
    else if ( key == "ftyp" ) newAtom = std::make_shared<ftyp>(atomBuild);
    else if ( key == "uuid" ) newAtom = std::make_shared<uuid>(atomBuild);
    else if ( key == "udta" ) newAtom = std::make_shared<udta>(atomBuild);
    else if ( key == "free" ) newAtom = std::make_shared<free>(atomBuild);
    else if ( key == "mdat" ) newAtom = std::make_shared<mdat>(atomBuild);
    else if ( key == "moov" ) newAtom = std::make_shared<moov>(atomBuild);
    else if ( key == "mvhd" ) newAtom = std::make_shared<mvhd>(atomBuild);
    else if ( key == "trak" ) newAtom = std::make_shared<trak>(atomBuild);
    else if ( key == "tkhd" ) newAtom = std::make_shared<tkhd>(atomBuild);
    else if ( key == "tref" ) newAtom = std::make_shared<tref>(atomBuild);
    else if ( key == "tmcd" ) newAtom = std::make_shared<tmcd>(atomBuild);
    else if ( key == "edts" ) newAtom = std::make_shared<edts>(atomBuild);
    else if ( key == "elst" ) newAtom = std::make_shared<elst>(atomBuild);
    else if ( key == "mdia" ) newAtom = std::make_shared<mdia>(atomBuild);
    else if ( key == "mdhd" ) newAtom = std::make_shared<mdhd>(atomBuild);
    else if ( key == "hdlr" ) newAtom = std::make_shared<hdlr>(atomBuild);
    else if ( key == "minf" ) newAtom = std::make_shared<minf>(atomBuild);
    else if ( key == "gmhd" ) newAtom = std::make_shared<gmhd>(atomBuild);
    else if ( key == "gmin" ) newAtom = std::make_shared<gmin>(atomBuild);
    else if ( key == "tcmi" ) newAtom = std::make_shared<tcmi>(atomBuild);
    else if ( key == "vmhd" ) newAtom = std::make_shared<vmhd>(atomBuild);
    else if ( key == "smhd" ) newAtom = std::make_shared<smhd>(atomBuild);
    else if ( key == "gpmd" ) newAtom = std::make_shared<gpmd>(atomBuild);
    else if ( key == "dinf" ) newAtom = std::make_shared<dinf>(atomBuild);
    else if ( key == "dref" ) newAtom = std::make_shared<dref>(atomBuild);
    else if ( key == "alis" ) newAtom = std::make_shared<alis>(atomBuild);
    else if ( key == "url " ) newAtom = std::make_shared<url_>(atomBuild);
    else if ( key == "stbl" ) newAtom = std::make_shared<stbl>(atomBuild);
    else if ( key == "stsd" ) newAtom = std::make_shared<stsd>(atomBuild);
    else if ( key == "stts" ) newAtom = std::make_shared<stts>(atomBuild);
    else if ( key == "stsc" ) newAtom = std::make_shared<stsc>(atomBuild);
    else if ( key == "stsz" ) newAtom = std::make_shared<stsz>(atomBuild);
    else if ( key == "stco" ) newAtom = std::make_shared<stco>(atomBuild);
    else if ( key == "co64" ) newAtom = std::make_shared<co64>(atomBuild);
    else if ( key == "stss" ) newAtom = std::make_shared<stss>(atomBuild);
    else newAtom = std::make_shared<atom>(atomBuild);

    return newAtom;
}

bool MP4::atom::isContainer_(std::ifstream &fileStream, int64_t dataSize)
{
    int64_t startPos = fileStream.tellg(), nextPos = startPos;
    fileStream.seekg(0, fileStream.end);
    int64_t fileSize = fileStream.tellg();
    fileStream.seekg(startPos, fileStream.beg);

    headerBlock dataBlock;
    int64_t size, totalSize = 0;
    bool result = false;
    do {
        fileStream.read((char *) &dataBlock, sizeof(dataBlock));
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
        fileStream.seekg(nextPos, fileStream.beg);
    } while ( totalSize < dataSize );

    fileStream.seekg(startPos, fileStream.beg);
    return result;
}

void MP4::atom::getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>> &found)
{
    for ( auto child : children_ ) {
        if ( child->key == findKey ) found.push_back(child);
        child->getChildAtoms_(findKey, found);
    }
}
