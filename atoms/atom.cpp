#include "atom.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <iomanip>

MP4::atom::atom(std::string filePath, uint64_t filePos, std::string pathParent)
    : filePath_(filePath)
    , filePos_(filePos)
    , parentPath_(pathParent)
{
    int64_t fileSize, childFilePos;
    bool container;

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("Atom can not parse file: "+filePath_);

    // get file size anset file position to start of atom
    fileStream.seekg(0, fileStream.end);
    fileSize = fileStream.tellg();
    fileStream.seekg(filePos, fileStream.beg);

    // read the header
    datablock::atomHeaderBlock dataBlock;
    fileStream.read((char *) &dataBlock, sizeof(dataBlock));

    key = std::string(dataBlock.key).substr(0,4);
    path_ = parentPath_ + key;
    #ifdef MP4_PARSE_PATH
    std::cout << path_ << std::endl;
    #endif

    // get atom size and data position
    dataBlock.size32 = _byteswap_ulong(dataBlock.size32);     // big to little endian
    dataBlock.size64 = _byteswap_uint64(dataBlock.size64);    // big to little endian
    if ( dataBlock.size32 == 1 ) {
        size_ = (int64_t) dataBlock.size64;
        fileDataPos_ = filePos + 16;
    } else {
        size_ = (int64_t) dataBlock.size32;
        fileDataPos_ = filePos + 8;
    }

    // set filestream to data position
    fileStream.seekg(fileDataPos_, fileStream.beg);

    // get other data
    fileNextPos_ = filePos + size_;
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
    childFilePos = fileDataPos_;
    do {
        auto child = makeAtom_(filePath_, childFilePos, path_+"/");
        if ( child != nullptr ) {
            childFilePos = child->fileNextPos_;
            children_.push_back(child);
        }
    } while ( childFilePos < fileNextPos_ );
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
    std::cout << path_ << " (Atom)" << std::endl;
    std::cout << dataIndent << "No data defined ..." << std::endl;
}

void MP4::atom::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::atom::writeToFile(std::ofstream &fileWrite, char *data)
{
    writeToFile_(fileWrite, data);
}

void MP4::atom::writeChildrenToFile_(std::ofstream &fileWrite, char *data)
{
    for ( auto child : children_ )
        child->writeToFile(fileWrite, data);
}

void MP4::atom::writeToFile_(std::ofstream &fileWrite, char *data)
{
    char    *buffer;
    size_t  bufferSize;
    
    if ( filePath_ == "" ) return;

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeFile can not parse file: "+filePath_);
    fileRead.seekg(filePos_, fileRead.beg);

    auto writeSizePos = fileWrite.tellp();
    bufferSize = (size_t) (fileDataPos_ - filePos_);
    buffer = new char[bufferSize];
    fileRead.read(buffer, bufferSize);
    fileWrite.write(buffer, bufferSize);
    delete[] buffer;

    if ( children_.size() == 0 ) {
        bufferSize = (size_t) dataSize_;
        buffer = new char[bufferSize];
        fileRead.seekg(fileDataPos_, fileRead.beg);
        fileRead.read(buffer, (size_t) bufferSize);
        fileWrite.write(buffer, (size_t) bufferSize);
        fileRead.close();
        delete[] buffer;
        return;
    }
    fileRead.close();

    writeChildrenToFile_(fileWrite, data);

    // set resulting atom size
    auto writeNextPos = fileWrite.tellp();
    auto writeSize = (uint32_t) (writeNextPos - writeSizePos);
    writeSize = _byteswap_ulong(writeSize);
    fileWrite.seekp(writeSizePos, fileWrite.beg);
    fileWrite.write((char *) &writeSize, sizeof(writeSize));
    fileWrite.seekp(writeNextPos, fileWrite.beg);
}

std::shared_ptr<MP4::atom>   MP4::atom::makeAtom_(std::string filePath_, int64_t nextFilePos, std::string pathParent)
{
    std::shared_ptr<atom> newAtom;

    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("Atom can not open file: "+filePath_);

    // get file size anset file position to start of atom
    fileStream.seekg(nextFilePos, fileStream.beg);
    char    charKey[4];;
    fileStream.read((char *) &charKey, sizeof(charKey));
    fileStream.read((char *) &charKey, sizeof(charKey));
    fileStream.close();
    std::string key = std::string(charKey).substr(0,4);

    if ( key == "ftyp" ) newAtom = std::make_shared<ftyp>(filePath_, nextFilePos, pathParent);
    else if ( key == "uuid" ) newAtom = std::make_shared<uuid>(filePath_, nextFilePos, pathParent);
    else if ( key == "mdat" ) newAtom = std::make_shared<mdat>(filePath_, nextFilePos, pathParent);
    else if ( key == "moov" ) newAtom = std::make_shared<moov>(filePath_, nextFilePos, pathParent);
    else if ( key == "mvhd" ) newAtom = std::make_shared<mvhd>(filePath_, nextFilePos, pathParent);
    else if ( key == "trak" ) newAtom = std::make_shared<trak>(filePath_, nextFilePos, pathParent);
    else if ( key == "tkhd" ) newAtom = std::make_shared<tkhd>(filePath_, nextFilePos, pathParent);
    else if ( key == "edts" ) newAtom = std::make_shared<edts>(filePath_, nextFilePos, pathParent);
    else if ( key == "elst" ) newAtom = std::make_shared<elst>(filePath_, nextFilePos, pathParent);
    else if ( key == "mdia" ) newAtom = std::make_shared<mdia>(filePath_, nextFilePos, pathParent);
    else if ( key == "mdhd" ) newAtom = std::make_shared<mdhd>(filePath_, nextFilePos, pathParent);
    else if ( key == "hdlr" ) newAtom = std::make_shared<hdlr>(filePath_, nextFilePos, pathParent);
    else if ( key == "minf" ) newAtom = std::make_shared<minf>(filePath_, nextFilePos, pathParent);
    else if ( key == "dinf" ) newAtom = std::make_shared<dinf>(filePath_, nextFilePos, pathParent);
    else if ( key == "dref" ) newAtom = std::make_shared<dref>(filePath_, nextFilePos, pathParent);
    else if ( key == "stbl" ) newAtom = std::make_shared<stbl>(filePath_, nextFilePos, pathParent);
    else if ( key == "stsd" ) newAtom = std::make_shared<stsd>(filePath_, nextFilePos, pathParent);
    else if ( key == "stts" ) newAtom = std::make_shared<stts>(filePath_, nextFilePos, pathParent);
    else if ( key == "stsc" ) newAtom = std::make_shared<stsc>(filePath_, nextFilePos, pathParent);
    else if ( key == "stsz" ) newAtom = std::make_shared<stsz>(filePath_, nextFilePos, pathParent);
    else if ( key == "stco" ) newAtom = std::make_shared<stco>(filePath_, nextFilePos, pathParent);
    else if ( key == "co64" ) newAtom = std::make_shared<co64>(filePath_, nextFilePos, pathParent);
    else if ( key == "stss" ) newAtom = std::make_shared<stss>(filePath_, nextFilePos, pathParent);
    else newAtom = std::make_shared<atom>(filePath_, nextFilePos, pathParent);

    return newAtom;
}

bool MP4::atom::isContainer_(std::ifstream &fileStream, int64_t dataSize)
{
    int64_t startPos = fileStream.tellg(), nextPos = startPos;
    fileStream.seekg(0, fileStream.end);
    int64_t fileSize = fileStream.tellg();
    fileStream.seekg(startPos, fileStream.beg);

    datablock::atomHeaderBlock dataBlock;
    int64_t size, totalSize = 0;
    bool result = false;
    do {
        fileStream.read((char *) &dataBlock, sizeof(dataBlock));
        dataBlock.size32 = _byteswap_ulong(dataBlock.size32);     // big to little endian
        dataBlock.size64 = _byteswap_uint64(dataBlock.size64);    // big to little endian
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
