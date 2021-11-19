#include "atom.hpp"
#include "root.hpp"
#include "ftyp.hpp"
#include "uuid.hpp"
#include "udta.hpp"
#include "free.hpp"
#include "FIRM.hpp"
#include "LENS.hpp"
#include "CAME.hpp"
#include "SETT.hpp"
#include "MUID.hpp"
#include "HMMT.hpp"
#include "BCID.hpp"
#include "GUMI.hpp"
#include "GPMF.hpp"
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
#include "ctts.hpp"

#include <iostream>
#include <iomanip>
#include "../date.hpp"

MP4::atom::atom()
    : key_("atom")
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

MP4::atom::atom(atomParse& parse)
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
    // get atom size and data position
    headerBlock dataBlock;
    fileStream->read((char*) &dataBlock, 8);
    size_ = (int64_t) XXH_swap32(dataBlock.size32);     // big to little endian
    headerSize64_ = false;
    headerSize_ = 8;
    if ( size_ == 1 ) {
        headerSize64_ = true;
        headerSize_ = 16;
        fileStream->seekg(filePos_, fileStream->beg);
        fileStream->read((char*) &dataBlock, sizeof(dataBlock));
        size_ = XXH_swap64(dataBlock.size64);
    }
    fileDataPos_ = filePos_ + headerSize_;

    // set key and path
    key_ = std::string(dataBlock.key).substr(0,4);
    path_ = parentPath_ + key_;

    // set filestream to data position
    fileStream->seekg(fileDataPos_, fileStream->beg);

    // get other data
    fileNextPos_ = filePos_ + size_;
    dataSize_ = fileNextPos_ - fileDataPos_;
    container = isContainer_(fileStream, dataSize_);
    fileStream->seekg(fileDataPos_, fileStream->beg);

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
    , filePos_(0)
    , fileDataPos_(0)
    , fileNextPos_(0)
    , dataSize_(0)
    , headerSize_(8)
    , moovAtom_(nullptr)
    , trakAtom_(nullptr)
    , build_(build)
{
    path_ = parentPath_ + getKey();
}

void MP4::atom::setMoov_(moov* moovAtom)
{
    moovAtom_ = moovAtom;
    for (auto child : children_ ) child->setMoov_(moovAtom);
}

void MP4::atom::setTrak_(trak* trakAtom)
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

std::string MP4::atom::getTimeString(uint32_t time, uint32_t timeScale)
{
    auto mseconds = time / (timeScale/1000);
    auto seconds = mseconds / 1000;
    auto mmseconds = mseconds - (seconds * 1000);
    std::string msecondsString = std::to_string(mmseconds);
    msecondsString = std::string(3 - msecondsString.length(), '0') + msecondsString;
    return (date::format("%T", std::chrono::seconds(seconds)) + "." + msecondsString);
}

std::string MP4::atom::getDateTimeString(uint32_t seconds)
{
    auto newSeconds = std::chrono::sys_seconds(std::chrono::seconds(seconds) - std::chrono::seconds(OFFSET_1904_1970));
    return date::format("%m/%d/%Y %T", newSeconds);
}

std::string MP4::atom::getZeroTerminatedString(std::string dataString, size_t maxLength)
{
    std::stringstream charss;
    size_t length = 0;
    for ( auto charByte : dataString ) {
        if ( maxLength != 0 && length > maxLength ) break;
        else if ( charByte == 0 ) break;
        charss << charByte;
        length++;
    }
    return charss.str();
}

uint32_t MP4::atom::getCurrentDateTime()
{
    auto utcTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto diffTime = std::mktime(std::gmtime(&utcTime)) - utcTime;
    auto currentSeconds = (uint32_t) (utcTime - diffTime);
    uint32_t newSeconds = OFFSET_1904_1970 + currentSeconds;
    return newSeconds;
}

uint32_t MP4::atom::timeScaleDuration(uint32_t duration, uint32_t sourceTimeScale, uint32_t targetTimeScale)
{
    auto timeScaleMult = (double) targetTimeScale / sourceTimeScale;
    return (uint32_t) (timeScaleMult * (double) duration);
}

void MP4::atom::dataStringViz(std::string& dataString, std::string indent)
{
    int lineCount = 0;
    std::stringstream charss;
    charss << std::hex;
    for ( auto charByte : dataString ) {
        //std::cout << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) charByte << std::endl;
        auto intVal = (int) (uint8_t) charByte;
        if (intVal >= 0x20 && intVal <= 0x7f ) {
            charss << charByte << "  ";
        } else {
            charss << std::setfill('0') << std::setw(2) << intVal << " ";
        }
        lineCount++;
        if ( lineCount == 8 ) {
            std::cout << indent << charss.str() << std::endl;
            charss.str("");
            lineCount = 0;
        }
    }
    std::cout << indent << charss.str() << std::endl;
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

std::string MP4::atom::getKey() const
{
    return key_;
}

void MP4::atom::getUserData(std::map<std::string, std::string>& userData)
{
    for ( auto child : children_ ) child->getUserData(userData);
}

void MP4::atom::write(std::shared_ptr<atomWriteFile> writeFile)
{
    write_(writeFile);
}

void MP4::atom::copy(std::shared_ptr<atomCopyFile> copyFile)
{
    if ( copyFile->isAtomExcluded(path_)) return;
    copy_(copyFile);
}

void MP4::atom::write_(std::shared_ptr<atomWriteFile> writeFile)
{
    writeHeader(writeFile);

    // decide how to handle data depending on children
    if ( children_.size() == 0 )
        writeData(writeFile);
    else
        writeChildren(writeFile);

    writeTail(writeFile);
}

void MP4::atom::copy_(std::shared_ptr<atomCopyFile> copyFile)
{
    copyHeader(copyFile);

    // decide how to handle data depending on children
    if ( children_.size() == 0 )
        copyData(copyFile);
    else
        copyChildren(copyFile);

    copyTail(copyFile);
}

void MP4::atom::writeHeader(std::shared_ptr<atomWriteFile> writeFile)
{
    writeHeader_(writeFile);
}

void MP4::atom::copyHeader(std::shared_ptr<atomCopyFile> copyFile)
{
    copyHeader_(copyFile);
}

void MP4::atom::writeHeader_(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    writeHeaderSizePos_ = fileWrite->tellp();
    headerBlock  atomHeader;
    memcpy(&atomHeader.key, getKey().c_str(), 4);
    fileWrite->write((char*) &atomHeader, headerSize_);
}

void MP4::atom::copyHeader_(std::shared_ptr<atomCopyFile> copyFile)
{
    auto fileWrite = copyFile->getFileWrite();
    writeHeaderSizePos_ = fileWrite->tellp();
    headerBlock  atomHeader;
    memcpy(&atomHeader.key, getKey().c_str(), 4);
    fileWrite->write((char*) &atomHeader, headerSize_);
}

void MP4::atom::writeChildren(std::shared_ptr<atomWriteFile> writeFile)
{
    writeChildren_(writeFile);
}

void MP4::atom::copyChildren(std::shared_ptr<atomCopyFile> copyFile)
{
    copyChildren_(copyFile);
}

void MP4::atom::writeChildren_(std::shared_ptr<atomWriteFile> writeFile)
{
    for ( auto child : children_ ) child->write(writeFile);
}

void MP4::atom::copyChildren_(std::shared_ptr<atomCopyFile> copyFile)
{
    for ( auto child : children_ ) child->copy(copyFile);
}

void MP4::atom::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    writeData_(writeFile);
}

void MP4::atom::copyData(std::shared_ptr<atomCopyFile> copyFile)
{
    copyData_(copyFile);
}

void MP4::atom::writeData_(std::shared_ptr<atomWriteFile> writeFile)
{
}

void MP4::atom::copyData_(std::shared_ptr<atomCopyFile> copyFile)
{
    auto fileWrite = copyFile->getFileWrite();

    auto fileRead = std::ifstream(filePath_, std::ios::binary);
    if ( fileRead.fail() ) error_("copyData_ can not read MP4 file: "+filePath_);

    fileRead.seekg(fileDataPos_, fileRead.beg);

    // write buffer blocks
    // this seems to be the most optimal block size
    size_t bufferSize = 1024*512;
    auto buffer = new char[bufferSize];
    auto bufferCount = (size_t) dataSize_ / bufferSize;

    for ( size_t count = 0; count < bufferCount; count++ ) {
        fileRead.read(buffer, bufferSize);
        fileWrite->write(buffer, bufferSize);
    }
    delete[] buffer;

    // write buffer rest
    bufferSize = (size_t) dataSize_ % bufferSize;
    buffer = new char[bufferSize];
    fileRead.read(buffer, bufferSize);
    fileWrite->write(buffer, bufferSize);
    delete[] buffer;

    fileRead.close();
}

void MP4::atom::writeTail(std::shared_ptr<atomWriteFile> writeFile)
{
    writeTail_(writeFile);
}

void MP4::atom::copyTail(std::shared_ptr<atomCopyFile> copyFile)
{
    copyTail_(copyFile);
}

void MP4::atom::writeTail_(std::shared_ptr<atomWriteFile> writeFile)
{
    // after writing the atom's data we need to fix the atom size
    auto fileWrite = writeFile->getFileWrite();
    auto writeNextPos = fileWrite->tellp();
    uint32_t size32;
    uint64_t size64;
    if ( headerSize_ == 16 ) {
        size32 = XXH_swap32((uint32_t) 1);
        size64 = writeNextPos - writeHeaderSizePos_;
        size64 = XXH_swap64(size64);
        fileWrite->seekp(writeHeaderSizePos_, fileWrite->beg);
        fileWrite->write((char*) &size32, sizeof(size32));
        fileWrite->seekp(4, fileWrite->cur);
        fileWrite->write((char*) &size64, sizeof(size64));
    } else if ( headerSize_ == 8 ) {
        size32 = (uint32_t) (writeNextPos - writeHeaderSizePos_);
        size32 = XXH_swap32(size32);
        fileWrite->seekp(writeHeaderSizePos_, fileWrite->beg);
        fileWrite->write((char*) &size32, sizeof(size32));
    }
    fileWrite->seekp(writeNextPos, fileWrite->beg);
}

void MP4::atom::copyTail_(std::shared_ptr<atomCopyFile> copyFile)
{
    // after writing the atom's data we need to fix the atom size
    auto fileWrite = copyFile->getFileWrite();
    auto writeNextPos = fileWrite->tellp();
    uint32_t size32;
    uint64_t size64;
    if ( headerSize_ == 16 ) {
        size32 = XXH_swap32((uint32_t) 1);
        size64 = writeNextPos - writeHeaderSizePos_;
        size64 = XXH_swap64(size64);
        fileWrite->seekp(writeHeaderSizePos_, fileWrite->beg);
        fileWrite->write((char*) &size32, sizeof(size32));
        fileWrite->seekp(4, fileWrite->cur);
        fileWrite->write((char*) &size64, sizeof(size64));
    } else if ( headerSize_ == 8 ) {
        size32 = (uint32_t) (writeNextPos - writeHeaderSizePos_);
        size32 = XXH_swap32(size32);
        fileWrite->seekp(writeHeaderSizePos_, fileWrite->beg);
        fileWrite->write((char*) &size32, sizeof(size32));
    }
    fileWrite->seekp(writeNextPos, fileWrite->beg);
}

void MP4::atom::error_(std::string message)
{
    std::cout << "atom: "<< std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}

std::shared_ptr<MP4::atom> MP4::atom::makeAtom_(atomParse& parse)
{
    std::shared_ptr<atom> newAtom;

    auto fileStream = parse.getFileStream();

    char charKey[4];
    auto filePos = fileStream->tellg();
    fileStream->seekg(4, fileStream->cur);
    fileStream->read((char*) &charKey, sizeof(charKey));
    fileStream->seekg(filePos, fileStream->beg);

    std::string key = std::string(charKey).substr(0,4);

    if ( key == "ftyp" ) newAtom = std::make_shared<ftyp>(parse);
    else if ( key == "uuid" ) newAtom = std::make_shared<uuid>(parse);
    else if ( key == "udta" ) newAtom = std::make_shared<udta>(parse);
    else if ( key == "free" ) newAtom = std::make_shared<free>(parse);
    else if ( key == "FIRM" ) newAtom = std::make_shared<FIRM>(parse);
    else if ( key == "LENS" ) newAtom = std::make_shared<LENS>(parse);
    else if ( key == "CAME" ) newAtom = std::make_shared<CAME>(parse);
    else if ( key == "SETT" ) newAtom = std::make_shared<SETT>(parse);
    else if ( key == "MUID" ) newAtom = std::make_shared<MUID>(parse);
    else if ( key == "HMMT" ) newAtom = std::make_shared<HMMT>(parse);
    else if ( key == "BCID" ) newAtom = std::make_shared<BCID>(parse);
    else if ( key == "GUMI" ) newAtom = std::make_shared<GUMI>(parse);
    else if ( key == "GPMF" ) newAtom = std::make_shared<GPMF>(parse);
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
    else if ( key == "ctts" ) newAtom = std::make_shared<ctts>(parse);
    else newAtom = std::make_shared<atom>(parse);

    return newAtom;
}

bool MP4::atom::isContainer_(std::ifstream *fileStream, int64_t dataSize)
{
    bool result = false;

    if ( dataSize < 8 ) return result;
    
    int64_t startPos = fileStream->tellg(), nextPos = startPos;
    fileStream->seekg(0, fileStream->end);
    int64_t fileSize = fileStream->tellg();
    fileStream->seekg(startPos, fileStream->beg);

    headerBlock dataBlock;
    int64_t size, totalSize = 0;
    do {
        fileStream->read((char*) &dataBlock, 8);
        size = (int64_t) XXH_swap32(dataBlock.size32);     // big to little endian
        if ( size == 1 ) {
            fileStream->seekg(nextPos, fileStream->beg);
            fileStream->read((char*) &dataBlock, sizeof(dataBlock));
            size = XXH_swap64(dataBlock.size64);
        }

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

void MP4::atom::getChildAtoms_(std::string findKey, std::vector<std::shared_ptr<atom>>& found)
{
    for ( auto child : children_ ) {
        if ( child->key_ == findKey ) found.push_back(child);
        child->getChildAtoms_(findKey, found);
    }
}
