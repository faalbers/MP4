#include "Writer.hpp"
#include <iostream>
#include "atoms/atom.hpp"
#include "atoms/atomWriteFile.hpp"
#include <fstream>
#include <filesystem>

MP4::Writer::Writer(Parser &parser)
{
}

MP4::Writer::Writer(Processor &processor)
{
    /*
    if ( processor.timeScale_ == 0 )
        error_("Writer: Processor timeScale is 0 !");
    timeScale_ = processor.timeScale_;
    duration_ = processor.duration_;
    for ( auto track : processor.tracks_ ) {
        tracks_[track.first] = track.second;
    }
    */
    try {
        auto build = std::make_shared<atomBuild>(processor);
        //filePath_ = build.getFilePath();
        rootAtom_ = std::make_shared<root>(build);
    } catch (std::runtime_error &error) {
        error_(error.what());
    }

}

std::string MP4::Writer::write(std::string fileName)
{
    std::string filePath = std::filesystem::absolute(std::filesystem::path(fileName)).string();

    std::ofstream fileWrite(filePath, std::ios::binary);
    if ( fileWrite.fail() ) error_("Can not write MP4 file: "+filePath);

    //auto fileWrite = std::make_shared<atomWriteFile>(fileName);

    rootAtom_->write(fileWrite);

    //splunk.fileWrite = &fileWrite;

    //rootAtom_->create(splunk);

    fileWrite.close();

    return filePath;
}

void MP4::Writer::error_(std::string message)
{
    std::cout << "Writer:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
