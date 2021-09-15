#include "mvhd.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::mvhd::mvhd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("mvhd atom can not parse file: "+filePath_);
    datablock::mvhdDataBlock mvhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &mvhdData, sizeof(mvhdData));
    timeScale = _byteswap_ulong(mvhdData.timeScale);
    duration = _byteswap_ulong(mvhdData.duration);
    mvhdData.preferredRate = _byteswap_ulong(mvhdData.preferredRate);
    preferredRate = (float)mvhdData.preferredRate / (float)(1 << 16);
    mvhdData.preferredVolume = _byteswap_ushort(mvhdData.preferredVolume);
    preferredVolume = (float)mvhdData.preferredVolume / (float)(1 << 8);
    
    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 2; j++) {
            mvhdData.matrix[i][j] = _byteswap_ulong(mvhdData.matrix[i][j]);
            matrix[i][j] = (float)mvhdData.matrix[i][j] / (float)(1 << 16);
        }
        mvhdData.matrix[i][2] = _byteswap_ulong(mvhdData.matrix[i][2]);
        matrix[i][2] = (float)mvhdData.matrix[i][2] / (float)(1 << 30);
    }
    
    nextTrackID = _byteswap_ulong(mvhdData.nextTrackID);

    fileStream.close();
}

void MP4::mvhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Movie Header Atom)" << std::endl;
    std::cout << dataIndent << "timeScale      : " << timeScale << std::endl;
    std::cout << dataIndent << "duration       : " << duration << std::endl;
    std::cout << dataIndent << "preferredRate  : " << preferredRate << std::endl;
    std::cout << dataIndent << "preferredVolume: " << preferredVolume << std::endl;
    std::cout << dataIndent << "matrix         :" << std::endl;
    std::cout << dataIndent << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << std::endl;
    std::cout << dataIndent << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << std::endl;
    std::cout << dataIndent << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << std::endl;
    std::cout << dataIndent << "nextTrackID    : " << nextTrackID << std::endl;
}

void MP4::mvhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::mvhd::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    // add append duration
    auto movieDuration = duration;
    auto timeScaleMult = (double) ((mvhd *)appendAtom)->timeScale / timeScale;
    movieDuration += (uint32_t) (((mvhd *)appendAtom)->duration / timeScaleMult);

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeAtomDataToFile_ can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);
    datablock::mvhdDataBlock mvhdData;
    fileRead.read((char *) &mvhdData, sizeof(mvhdData));
    fileRead.close();

    mvhdData.duration = _byteswap_ulong(movieDuration);
    
    fileWrite.write((char *) &mvhdData, sizeof(mvhdData));
}

std::string MP4::mvhd::key = "mvhd";

