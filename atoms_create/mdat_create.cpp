#include "mdat_create.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

MP4::mdat_create::mdat_create(splunkType &splunk)
    : atom_create("mdat", splunk.fileWrite)
{
    createHeader_(true);

    if (  splunk.samples.size() > 0 ) {
        std::string readPath = splunk.samples[0].filePath;
        std::ifstream fileRead(readPath, std::ios::binary);
        if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat_create can not parse file: "+readPath);
        
        // find largest sample size
        size_t bufferSize = 0;
        for ( auto sample : splunk.samples )
            if ( sample.size > bufferSize ) bufferSize = sample.size;

        //now copy sample buffers
        auto buffer = new char[bufferSize];
        for ( size_t index = 0; index < splunk.samples.size(); index++ ) {
            if ( splunk.samples[index].filePath != readPath ) {
                readPath = splunk.samples[index].filePath;
                fileRead.close();
                fileRead = std::ifstream(readPath, std::ios::binary);
                if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat_create can not parse file: "+readPath);
            }
            fileRead.seekg(splunk.samples[index].filePos, fileRead.beg);
            fileRead.read(buffer, splunk.samples[index].size);
            splunk.samples[index].filePos = fileWrite_->tellp();
            splunk.samples[index].filePath = splunk.filePath;
            fileWrite_->write(buffer, splunk.samples[index].size);
        }
        delete[] buffer;
        
        fileRead.close();
    }

    setSize_();
}

