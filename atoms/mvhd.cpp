#include "mvhd.hpp"
#include <iostream>

MP4::mvhd::mvhd(atomParse& parse)
    : atom(parse)
{
    auto fileStream = parse.getFileStream();

    dataBlock mvhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &mvhdData, sizeof(mvhdData));
    creationTime = XXH_swap32(mvhdData.creationTime);
    modificationTime = XXH_swap32(mvhdData.modificationTime);
    timeScale = XXH_swap32(mvhdData.timeScale);
    duration = XXH_swap32(mvhdData.duration);
    mvhdData.preferredRate = XXH_swap32(mvhdData.preferredRate);
    preferredRate = (float)mvhdData.preferredRate / (float)(1 << 16);
    mvhdData.preferredVolume = XXH_swap16(mvhdData.preferredVolume);
    preferredVolume = (float)mvhdData.preferredVolume / (float)(1 << 8);
    
    for ( int i = 0; i < 3; i++ ) {
        std::vector<float> row;
        for ( int j = 0; j < 2; j++) {
            row.push_back((float) XXH_swap32(mvhdData.matrix[i][j]) / (float)(1 << 16));
        }
        row.push_back((float) XXH_swap32(mvhdData.matrix[i][2]) / (float)(1 << 30));
        matrix.push_back(row);
    }
    
    nextTrackID = XXH_swap32(mvhdData.nextTrackID);

}

MP4::mvhd::mvhd(std::shared_ptr<atomBuild> build)
    : atom(build)
    // use largest track mdhd timeScale
    , timeScale(build->getVideoTimeScale())
    , duration(build->getVideoDuration())
    , creationTime(build->getCreationTime())
    , modificationTime(build->getModificationTime())
    , nextTrackID(build->getNextTrackID())
{
}

void MP4::mvhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "creationTime    : " << getDateTimeString(creationTime) << std::endl;
    std::cout << dataIndent << "modificationTime: " << getDateTimeString(modificationTime) << std::endl;
    std::cout << dataIndent << "timeScale       : " << timeScale << std::endl;
    std::cout << dataIndent << "duration        : " << duration
        << " (" << getTimeString(duration, timeScale) << ")\n";
    std::cout << dataIndent << "preferredRate   : " << preferredRate << std::endl;
    std::cout << dataIndent << "preferredVolume : " << preferredVolume << std::endl;
    std::cout << dataIndent << "matrix          :" << std::endl;
    std::cout << dataIndent << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << std::endl;
    std::cout << dataIndent << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << std::endl;
    std::cout << dataIndent << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << std::endl;
    std::cout << dataIndent << "nextTrackID    : " << nextTrackID << std::endl;
}

std::string MP4::mvhd::getKey()
{
    return key;
}

void MP4::mvhd::writeData(std::shared_ptr<atomWriteFile> writeFile)
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock mvhdData;

    // default settings
    mvhdData.version.version = 0;
    mvhdData.version.flag[0] = 0;
    mvhdData.version.flag[1] = 0;
    mvhdData.version.flag[2] = 0;
    for ( int i = 0; i < 10; i++ ) mvhdData.reserved[i] = 0;

    // forced to value, check later if this needs a copy of original mp4
    mvhdData.preferredRate = XXH_swap32((uint32_t) (1.0 * (float)(1 << 16)));
    mvhdData.preferredVolume = XXH_swap16((uint16_t) (1.0 * (float)(1 << 8)));
    mvhdData.matrix[0][0] = XXH_swap32((uint32_t) (1.0 * (float)(1 << 16)));
    mvhdData.matrix[0][1] = 0;
    mvhdData.matrix[0][2] = 0;
    mvhdData.matrix[1][0] = 0;
    mvhdData.matrix[1][1] = XXH_swap32((uint32_t) (1.0 * (float)(1 << 16)));
    mvhdData.matrix[1][2] = 0;
    mvhdData.matrix[2][0] = 0;
    mvhdData.matrix[2][1] = 0;
    mvhdData.matrix[2][2] = XXH_swap32((uint32_t) (1.0 * (float)(1 << 30)));

    // data settings
    mvhdData.timeScale = XXH_swap32(timeScale);
    mvhdData.duration = XXH_swap32(duration);
    mvhdData.nextTrackID = XXH_swap32(nextTrackID);

    // we use the actual writing time as creation and modification time
    mvhdData.creationTime = XXH_swap32(getCurrentDateTime());
    mvhdData.modificationTime = XXH_swap32(getCurrentDateTime());

    /*
    // forced to value, check later if this needs a copy of original track
    mvhdData.alternateGroupID = 0;

    // data settings
    mvhdData.trackID = XXH_swap32(trackID);
    mvhdData.duration = XXH_swap32(duration);
    mvhdData.layer = XXH_swap16(layer);
    mvhdData.volume = XXH_swap16((uint16_t) (volume * (float)(1 << 8)));
    mvhdData.trackWidth = XXH_swap32((uint32_t) (trackWidth * (float)(1 << 16)));
    mvhdData.trackHeight =  XXH_swap32((uint32_t) (trackHeight * (float)(1 << 16)));
    */

    //memcpy(&ftypData.majorBrand, majorBrand.c_str(), 4);
    //ftypData.version = XXH_swap32(version);
    
    fileWrite->write((char*) &mvhdData, sizeof(mvhdData));
    /*
    for ( auto brand : compatibleBrands ) {
        fileWrite->write((char*) brand.c_str(), 4);
    }
    */
}

const std::string MP4::mvhd::key = "mvhd";

