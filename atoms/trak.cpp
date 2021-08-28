#include "trak.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::trak::trak(std::string filePath, uint64_t filePos, std::string pathParent)
    : atom(filePath, filePos, pathParent)
{
}

MP4::tkhd *MP4::trak::get_tkhd()
{
    auto tkhds = getTypeAtoms<tkhd>();
    if ( tkhds.size() > 0 ) return getTypeAtoms<tkhd>()[0];
    return nullptr;
}

MP4::mdhd *MP4::trak::get_mdhd()
{
    auto mdhds = getTypeAtoms<mdhd>();
    if ( mdhds.size() > 0 ) return getTypeAtoms<mdhd>()[0];
    return nullptr;
}

MP4::hdlr *MP4::trak::get_hdlr()
{
    auto hdlrs = getTypeAtoms<hdlr>();
    if ( hdlrs.size() > 0 ) return getTypeAtoms<hdlr>()[0];
    return nullptr;
}

MP4::stsd *MP4::trak::get_stsd()
{
    auto stsds = getTypeAtoms<stsd>();
    if ( stsds.size() > 0 ) return getTypeAtoms<stsd>()[0];
    return nullptr;
}

uint32_t MP4::trak::getID()
{
    auto tkhd = get_tkhd();
    if ( tkhd == nullptr ) return 0;
    return tkhd->trackID;
}

bool MP4::trak::isComponentType(std::string type)
{
    auto hdlr = get_hdlr();
    if ( hdlr == nullptr ) return false;
    if ( hdlr->componentType == type) return true;
    return false;
}

bool MP4::trak::isComponentSubType(std::string type)
{
    auto hdlr = get_hdlr();
    if ( hdlr == nullptr ) return false;
    if ( hdlr->componentSubType == type) return true;
    return false;
}

bool MP4::trak::hasSampleDataFormat(std::string format)
{
    auto stsd = get_stsd();
    if ( stsd == nullptr ) return false;
    for ( auto entry : stsd->stsdTable )
        if ( entry.dataFormat == format) return true;
    return false;
}

void MP4::trak::printData(bool fullLists)
{
    int levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Atom)" << std::endl;
    std::cout << dataIndent << "This is a container Atom ..." << std::endl;
}

void MP4::trak::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

std::string MP4::trak::key = "trak";

