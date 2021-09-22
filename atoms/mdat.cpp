#include "mdat.hpp"
#include "../atoms.hpp"
#include <iostream>
#include <map>

MP4::mdat::mdat(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    sampleDataPos = fileDataPos_;
    sampleDataSize = dataSize_;
}

void MP4::mdat::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Data Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "filePosition of sample data : " << sampleDataPos << std::endl;
    std::cout << dataIndent << "Size of sample data in bytes: " << sampleDataSize << std::endl;
}

void MP4::mdat::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::mdat::createHeader(splunkType &splunk)
{
    createHeaderNew_(splunk, key, true);
}

void MP4::mdat::createData(splunkType &splunk)
{
    if (  splunk.samples.size() == 0 ) {
        createData_(splunk);
        return;
    }
    
    // find largest sample size
    size_t bufferSize = 0;
    for ( auto sample : splunk.samples )
        if ( sample.size > bufferSize ) bufferSize = sample.size;

    std::string readPath = splunk.samples[0].filePath;
    std::ifstream fileRead(readPath, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat::createData can not parse file: "+readPath);

    auto buffer = new char[bufferSize];
    for ( size_t index = 0; index < splunk.samples.size(); index++ ) {
        if ( splunk.samples[index].filePath != readPath ) {
            readPath = splunk.samples[index].filePath;
            fileRead.close();
            fileRead = std::ifstream(readPath, std::ios::binary);
            if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat::createData can not parse file: "+readPath);
        }

        // read sample
        fileRead.seekg(splunk.samples[index].filePos, fileRead.beg);
        fileRead.read(buffer, splunk.samples[index].size);

        // update sample filePos and path
        splunk.samples[index].filePos = splunk.fileWrite->tellp();
        splunk.samples[index].filePath = splunk.fileWritePath;

        splunk.fileWrite->write(buffer, splunk.samples[index].size);
    }
    delete[] buffer;

    fileRead.close();
}

std::string MP4::mdat::key = "mdat";

