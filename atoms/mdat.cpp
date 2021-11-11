#include "mdat.hpp"
#include <iostream>
#include <map>

MP4::mdat::mdat(atomParse &parse)
    : atom(parse)
{
    sampleDataPos = fileDataPos_;
    sampleDataSize = dataSize_;
}

MP4::mdat::mdat(std::shared_ptr<atomBuild> build)
    : atom(build)
    , sampleDataPos(0)
    , sampleDataSize(0)
{
    headerSize_ = 16;
    path_ = parentPath_ + key;

    // create time sorted samples data
    std::map<uint32_t, std::vector< std::vector<uint32_t>>> mdatSamples;
    auto tracks = build->getTracks();
    for ( auto track : tracks) {
        for ( auto sample : track.second->samples ) {
            std::vector<uint32_t> mdatSample;
            auto sampleTime = atom::timeScaleDuration(
                sample.second.time,
                track.second->mediaTimeScale, track.second->videoTimeScale);
            mdatSample.push_back(track.first);
            mdatSample.push_back(sample.first);
            if ( mdatSamples.find(sample.second.time) == mdatSamples.end() ) {
                std::vector< std::vector<uint32_t>> entry;
                mdatSamples[sample.second.time] = entry;
            }
            mdatSamples[sample.second.time].push_back(mdatSample);
        }
    }

    // save time sorted mdat write data
    for ( auto mdatSample : mdatSamples ) {
        for ( auto sample : mdatSample.second ) {
            mdatWriteType_ writeEntry;
            writeEntry.filePath = tracks[sample[0]]->samples[sample[1]].filePath;
            writeEntry.filePos = tracks[sample[0]]->samples[sample[1]].filePos;
            writeEntry.size = (size_t) tracks[sample[0]]->samples[sample[1]].size;
            writeEntry.trackID = sample[0];
            writeEntry.sampleID = sample[1];
            mdatWrite_.push_back(writeEntry); 
        }
    }

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

std::string MP4::mdat::getKey()
{
    return key;
}

void MP4::mdat::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    // find largest sample buffer size
    auto fileWrite = writeFile->getFileWrite();
    size_t bufferSize = 0;
    for ( auto mdatEntry : mdatWrite_ )
        if ( mdatEntry.size > bufferSize ) bufferSize = mdatEntry.size;
    auto buffer = new char[bufferSize];
    std::string filePath = "";
    std::ifstream fileRead;
    for ( auto mdatEntry : mdatWrite_ ) {
        if ( mdatEntry.filePath != filePath ) {
            filePath = mdatEntry.filePath;
            if ( fileRead.is_open() ) fileRead.close();
            fileRead.open(filePath, std::ios::binary);
            if ( fileRead.fail() ) error_("mdat: constructer can not read file: "+filePath);
        }
        fileRead.seekg(mdatEntry.filePos, fileRead.beg);
        fileRead.read(buffer, mdatEntry.size);

        // write and set mdatWriteInfo
        writeFile->mdatWriteInfo[mdatEntry.trackID].push_back(fileWrite->tellp());
        fileWrite->write(buffer, mdatEntry.size);
    }
    if ( fileRead.is_open() ) fileRead.close();
    delete[] buffer;
}

std::string MP4::mdat::key = "mdat";

