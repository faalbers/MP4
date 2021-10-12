#include "Writer.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <atoms/atom.hpp>

MP4::Writer::Writer(Parser &parser)
{
}

MP4::Writer::Writer(Processor &processor)
{
    if ( processor.timeScale_ == 0 )
        error_("Writer: Processor timeScale is 0 !");
    timeScale_ = processor.timeScale_;
    duration_ = processor.duration_;
    for ( auto track : processor.tracks_ ) {
        tracks_[track.first] = track.second;
    }
}

std::string MP4::Writer::write(std::string fileName)
{
    std::string filePath = std::filesystem::absolute(std::filesystem::path(fileName)).string();

    return filePath;
}

void MP4::Writer::error_(std::string message)
{
    std::cout << "Writer:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
