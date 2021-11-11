#include "Writer.hpp"
#include <iostream>
#include "atoms/atom.hpp"
#include "atoms/atomWriteFile.hpp"

MP4::Writer::Writer(Parser &parser)
{
    rootAtomParser_ = parser.getRootAtom();
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
    auto fileWrite = std::make_shared<atomWriteFile>(fileName);

    rootAtom_->write(fileWrite);

    return fileWrite->getFilePath();
}

void MP4::Writer::copyTest(std::string fileName)
{
    // this is a test for now
}

void MP4::Writer::error_(std::string message)
{
    std::cout << "Writer:" << std::endl;
    std::cout << "-> " << message << std::endl;
    std::cout << "exit application ..." << std::endl;
    exit(1);
}
