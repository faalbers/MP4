#include "Parser.hpp"
#include <filesystem>
#include <memory>
#include "atoms/atom.hpp"
#include "atoms/trak.hpp"
#include "atoms/stsd.hpp"
#include "atoms/hdlr.hpp"
#include <iostream>

MP4::Parser::Parser(std::string fileName)
{
    try {
        atomBuild build(fileName);
        filePath_ = build.getFilePath();
        rootAtom_ = std::make_shared<root>(build);
    } catch (std::runtime_error &error) {
        std::cout << "Parser: " << error.what() << std::endl;
        std::cout << "exiting application ..." << std::endl;
        exit(1);
    }

    //filePath_ = std::filesystem::absolute(std::filesystem::path(fileName)).string();
    //atomBuildType atomBuild;
    //atomBuild.filePath = filePath_;
    //rootAtom_ = std::make_shared<root>(atomBuild);
}

void MP4::Parser::printHierarchy()
{
    int pathWith = rootAtom_->nestLevel_(0) * 5;
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

    rootAtom_->printHierarchy(pathWith, valWith);
    
    std::cout << std::endl;
}

void MP4::Parser::printHierarchyData(bool fullLists)
{
    //std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    std::cout << std::endl;
    rootAtom_->printHierarchyData(fullLists);
}

std::set<std::string> MP4::Parser::getDataFormats()
{
    std::set<std::string> dataFormats;
    for ( auto stsd : rootAtom_->getTypeAtoms<stsd>() ) {
        for ( auto dataFormat : stsd->getDataFormats() )
            dataFormats.insert(dataFormat);
    }
    return dataFormats;
}

std::set<std::string> MP4::Parser::getComponentTypes()
{
    std::set<std::string> componentSubTypes;
    for ( auto hdlr : rootAtom_->getTypeAtoms<hdlr>() )
        componentSubTypes.insert(hdlr->componentType);
    return componentSubTypes;
}

std::set<std::string> MP4::Parser::getComponentSubTypes()
{
    std::set<std::string> componentSubTypes;
    for ( auto hdlr : rootAtom_->getTypeAtoms<hdlr>() )
        componentSubTypes.insert(hdlr->componentSubType);
    return componentSubTypes;
}

std::set<uint32_t> MP4::Parser::getDataFormatTrackIDs(std::string dataFormat)
{
    std::set<uint32_t> trackIDs;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        if ( dataFormat != "" && !trak->isDataFormat(dataFormat) ) continue;
        trackIDs.insert(trak->getID());
    }
    return trackIDs;
}

std::set<uint32_t> MP4::Parser::getComponentSubTypeTrackIDs(std::string componentSubType)
{
    std::set<uint32_t> trackIDs;
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        if ( componentSubType != "" && !trak->isComponentSubType(componentSubType) ) continue;
        trackIDs.insert(trak->getID());
    }
    return trackIDs;
}

std::shared_ptr<MP4::trackType> MP4::Parser::getTrack(uint32_t trackID)
{
    for ( auto trak : rootAtom_->getTypeAtoms<trak>() ) {
        if ( trak->getID() == trackID ) return trak->getTrack();
    }
    warning_("TrackID: " + std::to_string(trackID) + " does not exist in mp4 file.");
    return nullptr;
}

void MP4::Parser::warning_(std::string message)
{
    std::cout << "Parser: " << filePath_ << std::endl;
    std::cout << "-> " << message << std::endl;
}