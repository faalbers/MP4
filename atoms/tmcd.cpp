#include "tmcd.hpp"
#include <iostream>

MP4::tmcd::tmcd(atomParse& parse)
    : atom(parse)
    , isTrackReference(false)
{
    // there are several atoms with the same key name.
    if (parentPath_.find("/tref") != std::string::npos) {
        isTrackReference = true;
    }

    auto fileStream = parse.getFileStream();

    fileStream->seekg(fileDataPos_, fileStream->beg);

    if ( isTrackReference ) {
        uint32_t trackID;
        do {
            fileStream->read((char*) &trackID, sizeof(trackID));
            trackIDs.push_back(XXH_swap32(trackID));
        } while ( fileStream->tellg() < fileNextPos_ );
    }

}

MP4::tmcd::tmcd(std::shared_ptr<atomBuild> build)
    : atom(build)
    , isTrackReference(false)
{
    if (parentPath_.find("/tref") != std::string::npos) {
        isTrackReference = true;
        trackIDs = build->getReferenceTrackIDs("tmcd");
    }
}

void MP4::tmcd::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    if ( isTrackReference ) {
        std::cout << path_ << " (Track Reference Atom) ["<< headerSize_ << "]" << std::endl;
        std::cout << dataIndent << "[#] (trackID)\n";
        size_t index = 1;
        for ( auto entry : trackIDs ) {
            std::cout << dataIndent << "[" << index << "] ( " << entry << " )" << std::endl;
            index++;
        }
    } else {
        std::cout << path_ << " (Timecode Sample Description Atom) ["<< headerSize_ << "]" << std::endl;
        std::cout << dataIndent << "This is a container Atom ..." << std::endl;
    }
}

std::string MP4::tmcd::getKey() const
{
    return key;
}

void MP4::tmcd::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    for ( auto trackID : trackIDs ) {
        trackID = XXH_swap32(trackID);
        fileWrite->write((char*) &trackID, sizeof(trackID));
    }
}

const std::string MP4::tmcd::key = "tmcd";

