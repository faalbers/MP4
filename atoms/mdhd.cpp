#include "mdhd.hpp"
#include <iostream>

MP4::mdhd::mdhd(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    dataBlock mdhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &mdhdData, sizeof(mdhdData));
    creationTime = XXH_swap32(mdhdData.creationTime);
    modificationTime = XXH_swap32(mdhdData.modificationTime);
    timeScale = XXH_swap32(mdhdData.timeScale);
    duration = XXH_swap32(mdhdData.duration);
    language = XXH_swap16(mdhdData.language);
    quality = XXH_swap16(mdhdData.quality);
}

MP4::mdhd::mdhd(std::shared_ptr<atomBuild> build)
    : atom(build)
    // use tkhd for media date
    , creationTime(build->getTrackCreationTime())
    , modificationTime(build->getTrackModificationTime())
    , timeScale(build->getMediaTimeScale())
    , duration(build->getMediaDuration())
    , language(build->getMediaLanguage())
    , quality(build->getMediaQuality())
{
}

void MP4::mdhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Media Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "creationTime    : " << getDateTimeString(creationTime) << std::endl;
    std::cout << dataIndent << "modificationTime: " << getDateTimeString(modificationTime) << std::endl;
    std::cout << dataIndent << "timeScale       : " << timeScale << std::endl;
    std::cout << dataIndent << "duration        : " << duration
        << " (" << getTimeString(duration, timeScale) << ")\n";
    std::cout << dataIndent << "language        : " << language << std::endl;
    std::cout << dataIndent << "quality         : " << quality << std::endl;
}

std::string MP4::mdhd::getKey()
{
    return key;
}

void MP4::mdhd::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock mdhdData;

    // default settings
    mdhdData.version.version = 0;
    mdhdData.version.flag[0] = 0;
    mdhdData.version.flag[1] = 0;
    mdhdData.version.flag[2] = 0;

    // data settings
    mdhdData.creationTime = XXH_swap32(creationTime);
    mdhdData.modificationTime = XXH_swap32(modificationTime);
    mdhdData.timeScale = XXH_swap32(timeScale);
    mdhdData.duration = XXH_swap32(duration);
    mdhdData.language = XXH_swap16(language);
    mdhdData.quality = XXH_swap16(quality);

    fileWrite->write((char*) &mdhdData, sizeof(mdhdData));

}

const std::string MP4::mdhd::key = "mdhd";

