#include "stss.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::stss::stss(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("stss atom can not parse file: "+filePath_);
    datablock::atomTableBlock stssData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &stssData, sizeof(stssData));
    stssData.numberOfEntries = _byteswap_ulong(stssData.numberOfEntries);
    auto index = stssData.numberOfEntries;
    uint32_t numberTemp;
    do {
        fileStream.read((char *) &numberTemp, sizeof(numberTemp));
        stssTable.push_back(_byteswap_ulong(numberTemp));
        index--;
    } while ( index > 0);
    fileStream.close();
}

void MP4::stss::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Sync Sample Atom) ["<< headerSize_ << "]" << std::endl;
    size_t index = 1;
    std::cout << dataIndent << "[#] (sample number)\n";
    if ( fullLists || (!fullLists && stssTable.size() <= 6) ) {
        for ( auto entry : stssTable ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        for ( index = 0 ; index < 3; index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stssTable[index] << " )" << std::endl;
        }
        std::cout << dataIndent << "     ...\n";
        for ( index = stssTable.size()-3 ; index < stssTable.size(); index++ ) {
            std::cout << dataIndent << "[" << index+1 << "] ( " << stssTable[index] << " )" << std::endl;
        }
    }
}

void MP4::stss::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::stss::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    //writeData_(fileWrite, writeInfo);
    //return;

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeAtomDataToFile_ can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);
    datablock::atomTableBlock stssData;
    fileRead.seekg(fileDataPos_, fileRead.beg);
    fileRead.read((char *) &stssData, sizeof(stssData));
    fileRead.close();

    auto newTableSize = stssTable.size() + ((stss *) appendAtom)->stssTable.size();
    std::cout << newTableSize << std::endl;

    stssData.numberOfEntries = _byteswap_ulong((uint32_t) newTableSize);
    
    fileWrite.write((char *) &stssData, sizeof(stssData));

    for ( auto entry : stssTable ) {
        entry = _byteswap_ulong(entry);
        fileWrite.write((char *) &entry, sizeof(entry));
    }
    
    uint32_t sampleCountA = 0;
    auto trackID = trakAtom_->getID();
    for ( auto chunk : writeInfo.chunkLists[0] )
        if ( chunk->trackID == trackID) sampleCountA += chunk->samples;
    std::cout << sampleCountA << std::endl;
    for ( auto entry : ((stss *) appendAtom)->stssTable ) {
        entry = _byteswap_ulong(entry+sampleCountA);
        fileWrite.write((char *) &entry, sizeof(entry));
    }
}

void MP4::stss::createData(splunkType &splunk)
{
    //createData_(splunk);

    // no checking of trackID since that is done in the trak level

    auto trackID = trakAtom_->getID();

    datablock::atomTableBlock stssData;
    stssData.version = 0;
    stssData.flag[0] = 0;
    stssData.flag[1] = 0;
    stssData.flag[2] = 0;

    splunk.fileWrite->write((char *) &stssData, sizeof(stssData) );
    auto entriesSizePos = splunk.fileWrite->tellp() - (int64_t) 4;
    uint32_t syncCount = 0;
    for ( auto sample : splunk.samples ) {
        if ( sample.trackID == trackID && sample.sync ) {
            auto ID = _byteswap_ulong((uint32_t) sample.ID);
            splunk.fileWrite->write((char *) &ID, sizeof(ID));
            syncCount++;
        }
    }
    auto lastPos = splunk.fileWrite->tellp();
    splunk.fileWrite->seekp(entriesSizePos, splunk.fileWrite->beg);
    syncCount = _byteswap_ulong(syncCount);
    splunk.fileWrite->write((char *) &syncCount, sizeof(syncCount));
    splunk.fileWrite->seekp(lastPos, splunk.fileWrite->beg);
}

std::string MP4::stss::key = "stss";

