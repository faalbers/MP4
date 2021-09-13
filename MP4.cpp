#include "MP4.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <map>
#ifdef MP4_PARSE_TIME
#include <chrono>
#endif

MP4::MP4::MP4(std::string fileName)
{
    #ifdef MP4_PARSE_TIME
    auto start = std::chrono::high_resolution_clock::now();
    #endif
    
    filePath = std::filesystem::absolute(std::filesystem::path(fileName)).string();

    internal::atomBuildType atomBuild;
    atomBuild.filePath = filePath;
    rootAtom = std::make_shared<root>(atomBuild);
    /*
    do {
        atomBuild.parentPath = "/";
        auto child = atom::makeAtom_(atomBuild);
        atomBuild.filePos = child->fileNextPos_;
        children.push_back(child);
    } while ( atomBuild.filePos < fileSize );
    */
    
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

MP4::trak *MP4::MP4::getTrackFromID(uint32_t ID)
{
    for ( auto track : getTracks() )
        if ( track->getID() == ID ) return track;
    return nullptr;
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
    //std::cout << std::string(26, '-') << " MOVIE  " << std::string(26, '-') << std::endl;
    std::cout << std::endl;
    rootAtom->printHierarchyData(fullLists);
}

void MP4::MP4::write(std::string filePath_, writeSettingsType &writeSettings)
{
    std::ofstream fileWrite(filePath_, std::ios::binary);
    if ( fileWrite.fail() ) throw std::runtime_error("Can not write MP4 file: "+filePath_);

    // data to pass through write process
    internal::writeInfoType writeInfo;

    // passing moov as data to atoms to reconstruct stuff if needed
    //for ( auto moov : getTypeAtoms<moov>() )
    //    writeInfo.moovAtom = moov;
    writeInfo.mp4List.push_back(this);

    // exclude tracks with incorrect timing
    for ( auto moov : getTypeAtoms<moov>() )
        for ( auto track : moov->getTypeAtoms<trak>() )
            for ( auto mdhd : track->getTypeAtoms<mdhd>() )
                for ( auto stts : track->getTypeAtoms<stts>() ) {
                    uint32_t totalDuration = 0;
                    for ( auto entry : stts->sttsTable )
                        totalDuration = entry[0] * entry[1];
                    if ( mdhd->duration != totalDuration )
                        writeSettings.excludeTrackIDs.insert(track->getID());
                }
    
    // make final track include map
    for ( auto moov : getTypeAtoms<moov>() )
        for ( auto track : moov->getTypeAtoms<trak>() ) {
            std::set<uint32_t>::iterator it = writeSettings.excludeTrackIDs.find(track->getID());
            if( it != writeSettings.excludeTrackIDs.end() ) continue;
            writeInfo.includeTrackIDs[track->getID()] = track->getID();
        }

    // first write ftyp
    for ( auto child : getTypeAtoms<ftyp>() ) child->write(fileWrite, writeInfo);

    // write mdat 
    for ( auto child : getTypeAtoms<mdat>() ) child->write(fileWrite, writeInfo);

    fileWrite.close();
}

void MP4::MP4::append(MP4 &appendMP4, std::string filePath_, writeSettingsType &writeSettings)
{
    std::ofstream fileWrite(filePath_, std::ios::binary);
    if ( fileWrite.fail() ) throw std::runtime_error("Can not write MP4 file: "+filePath_);

    // data to pass through write process
    internal::writeInfoType writeInfo;

    // passing moov as data to atoms to reconstruct stuff if needed
    //for ( auto moov : getTypeAtoms<moov>() )
    //    writeInfo.moovAtom = moov;
    writeInfo.mp4List.push_back(this);
    writeInfo.mp4List.push_back(&appendMP4);

    // exclude tracks with incorrect timing
    for ( auto moov : getTypeAtoms<moov>() )
        for ( auto track : moov->getTypeAtoms<trak>() )
            for ( auto mdhd : track->getTypeAtoms<mdhd>() )
                for ( auto stts : track->getTypeAtoms<stts>() ) {
                    uint32_t totalDuration = 0;
                    for ( auto entry : stts->sttsTable )
                        totalDuration = entry[0] * entry[1];
                    if ( mdhd->duration != totalDuration )
                        writeSettings.excludeTrackIDs.insert(track->getID());
                }

    // make final track include map
    for ( auto moov : getTypeAtoms<moov>() )
        for ( auto track : moov->getTypeAtoms<trak>() ) {
            std::set<uint32_t>::iterator it = writeSettings.excludeTrackIDs.find(track->getID());
            if( it != writeSettings.excludeTrackIDs.end() ) continue;
            writeInfo.includeTrackIDs[track->getID()] = track->getID();
        }

    // first write ftyp, we used the main mp4 ftyp
    for ( auto child : getTypeAtoms<ftyp>() ) child->write(fileWrite, writeInfo);

    // combine mdat
    for ( auto child : getTypeAtoms<mdat>() ) {
        for ( auto appendChild : appendMP4.children )
            if ( appendChild->key == child->key )
                child->append(appendChild.get(), fileWrite, writeInfo);
    }

    fileWrite.close();
}
