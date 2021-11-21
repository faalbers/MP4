#include "tkhd.hpp"
#include "moov.hpp"
#include "mvhd.hpp"
#include <iostream>

MP4::tkhd::tkhd(atomParse& parse)
    : atom(parse)
{
    // get data
    auto fileStream = parse.getFileStream();

    dataBlock tkhdData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &tkhdData, sizeof(tkhdData));
    
    creationTime = XXH_swap32(tkhdData.creationTime);
    modificationTime = XXH_swap32(tkhdData.modificationTime);
    trackID = XXH_swap32(tkhdData.trackID);
    duration = XXH_swap32(tkhdData.duration);
    layer = XXH_swap16(tkhdData.layer);
    alternateGroupID = XXH_swap16(tkhdData.alternateGroupID);
    tkhdData.volume = XXH_swap16(tkhdData.volume);
    volume = (float)tkhdData.volume / (float)(1 << 8);
    tkhdData.trackWidth = XXH_swap32(tkhdData.trackWidth);
    trackWidth = (float)tkhdData.trackWidth / (float)(1 << 16);
    tkhdData.trackHeight = XXH_swap32(tkhdData.trackHeight);
    trackHeight = (float)tkhdData.trackHeight / (float)(1 << 16);

    for ( int i = 0; i < 3; i++ ) {
        std::vector<float> row;
        for ( int j = 0; j < 2; j++) {
            row.push_back((float) XXH_swap32(tkhdData.matrix[i][j]) / (float)(1 << 16));
        }
        row.push_back((float) XXH_swap32(tkhdData.matrix[i][2]) / (float)(1 << 30));
        matrix.push_back(row);
    }
    
}

MP4::tkhd::tkhd(std::shared_ptr<atomBuild> build)
    : atom(build)
    , trackID(build->getCurrentTrackID())
    , duration(build->getTrackDuration())
    , creationTime(build->getTrackCreationTime())
    , modificationTime(build->getTrackModificationTime())
    , layer(build->getTrackLayer())
    , volume(build->getTrackVolume())
    , trackWidth(build->getTrackWidth())
    , trackHeight(build->getTrackHeight())
    , matrix(build->getTrackMatrix())
{
}

void MP4::tkhd::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Header Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "creationTime    : " << getDateTimeString(creationTime) << std::endl;
    std::cout << dataIndent << "modificationTime: " << getDateTimeString(modificationTime) << std::endl;
    std::cout << dataIndent << "trackID          : " << trackID << std::endl;
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>() ) {
        std::cout << dataIndent << "duration         : " << duration
            << " (" << getTimeString(duration, mvhd->timeScale) << ")\n";
    }
    std::cout << dataIndent << "layer            : " << layer << std::endl;
    std::cout << dataIndent << "volume           : " << volume << std::endl;
    std::cout << dataIndent << "trackWidth       : " << trackWidth << std::endl;
    std::cout << dataIndent << "trackHeight      : " << trackHeight << std::endl;
    std::cout << dataIndent << "matrix           :" << std::endl;
    std::cout << dataIndent << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << std::endl;
    std::cout << dataIndent << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << std::endl;
    std::cout << dataIndent << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << std::endl;
}

std::string MP4::tkhd::getKey() const
{
    return key;
}

void MP4::tkhd::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    dataBlock tkhdData;

    // default settings
    tkhdData.version.version = 0;
    tkhdData.version.flag[0] = 0;
    tkhdData.version.flag[1] = 0;
    tkhdData.version.flag[2] = 15;
    tkhdData.reservedA = 0;
    for ( int i = 0; i < 8; i++ ) tkhdData.reservedB[i] = 0;
    tkhdData.reservedC = 0;

    // forced to value, check later if this needs a copy of original track
    tkhdData.alternateGroupID = 0;

    // data settings
    tkhdData.creationTime = XXH_swap32(creationTime);
    tkhdData.modificationTime = XXH_swap32(modificationTime);
    tkhdData.trackID = XXH_swap32(trackID);
    tkhdData.duration = XXH_swap32(duration);
    tkhdData.layer = XXH_swap16(layer);
    tkhdData.volume = XXH_swap16((uint16_t) (volume * (float)(1 << 8)));
    tkhdData.trackWidth = XXH_swap32((uint32_t) (trackWidth * (float)(1 << 16)));
    tkhdData.trackHeight =  XXH_swap32((uint32_t) (trackHeight * (float)(1 << 16)));
    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 2; j++) {
            tkhdData.matrix[i][j] = XXH_swap32((uint32_t) (matrix[i][j] * (float)(1 << 16)));
        }
        tkhdData.matrix[i][2] = XXH_swap32((uint32_t) (matrix[i][2] * (float)(1 << 30)));
    }

    //memcpy(&ftypData.majorBrand, majorBrand.c_str(), 4);
    //ftypData.version = XXH_swap32(version);
    
    fileWrite->write((char*) &tkhdData, sizeof(tkhdData));
    /*
    for ( auto brand : compatibleBrands ) {
        fileWrite->write((char*) brand.c_str(), 4);
    }
    */
}

const std::string MP4::tkhd::key = "tkhd";

