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
        
        // find largest sample size
        size_t bufferSize = 0;
        for ( auto sample : splunk.samples )
            if ( sample.size > bufferSize ) bufferSize = sample.size;

        //now copy sample buffers
        auto buffer = new char[bufferSize];
        if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat_create can not parse file: "+readPath);
        for ( auto sample : splunk.samples ) {
            if ( sample.filePath != readPath ) {
                readPath = sample.filePath;
                fileRead.close();
                fileRead = std::ifstream(readPath, std::ios::binary);
                if ( fileRead.fail() ) throw std::runtime_error("MP4::mdat_create can not parse file: "+readPath);
            }
            fileRead.seekg(sample.filePos, fileRead.beg);
            fileRead.read(buffer, sample.size);
            fileWrite_->write(buffer, sample.size);
        }
        delete[] buffer;
        
        fileRead.close();
    }

    setSize_();
}

