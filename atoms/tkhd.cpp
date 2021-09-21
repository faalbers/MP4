#include "tkhd.hpp"
#include "../atoms.hpp"
#include <iostream>

MP4::tkhd::tkhd(internal::atomBuildType &atomBuild)
    : atom(atomBuild)
{
    // get data
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("tkhd atom can not parse file: "+filePath_);
    datablock::tkhdDataBlock tkhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &tkhdData, sizeof(tkhdData));
    
    trackID = _byteswap_ulong(tkhdData.trackID);
    duration = _byteswap_ulong(tkhdData.duration);
    tkhdData.volume = _byteswap_ushort(tkhdData.volume);
    volume = (float)tkhdData.volume / (float)(1 << 8);
    tkhdData.trackWidth = _byteswap_ulong(tkhdData.trackWidth);
    trackWidth = (float)tkhdData.trackWidth / (float)(1 << 16);
    tkhdData.trackHeight = _byteswap_ulong(tkhdData.trackHeight);
    trackHeight = (float)tkhdData.trackHeight / (float)(1 << 16);

    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 0; j < 2; j++) {
            tkhdData.matrix[i][j] = _byteswap_ulong(tkhdData.matrix[i][j]);
            matrix[i][j] = (float)tkhdData.matrix[i][j] / (float)(1 << 16);
        }
        tkhdData.matrix[i][2] = _byteswap_ulong(tkhdData.matrix[i][2]);
        matrix[i][2] = (float)tkhdData.matrix[i][2] / (float)(1 << 30);
    }
    
    fileStream.close();
}

void MP4::tkhd::printData(bool fullLists)
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Track Header Atom)" << std::endl;
    std::cout << dataIndent << "trackID    : " << trackID << std::endl;
    std::cout << dataIndent << "duration   : " << duration << std::endl;
    std::cout << dataIndent << "volume     : " << volume << std::endl;
    std::cout << dataIndent << "trackWidth : " << trackWidth << std::endl;
    std::cout << dataIndent << "trackHeight: " << trackHeight << std::endl;
    std::cout << dataIndent << "matrix     :" << std::endl;
    std::cout << dataIndent << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << std::endl;
    std::cout << dataIndent << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << std::endl;
    std::cout << dataIndent << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << std::endl;
}

void MP4::tkhd::printHierarchyData(bool fullLists)
{
    printData(fullLists);
    for ( auto child : children_ ) child->printHierarchyData(fullLists);
}

void MP4::tkhd::appendData(atom *appendAtom, std::ofstream &fileWrite, internal::writeInfoType &writeInfo)
{
    uint32_t movieTimeScaleA;
    uint32_t movieDurationA;
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>()) {
        movieTimeScaleA = mvhd->timeScale;
        movieDurationA = mvhd->duration;
    }
    uint32_t movieTimeScaleB;
    for ( auto mvhd : ((tkhd *)appendAtom)->moovAtom_->getTypeAtoms<mvhd>())
        movieTimeScaleB  = mvhd->timeScale;

    // add append duration
    auto movieDuration = movieDurationA;
    auto timeScaleMultB = (double) movieTimeScaleB / movieTimeScaleA;
    movieDuration += (uint32_t) (((tkhd *)appendAtom)->duration / timeScaleMultB);

    std::ifstream fileRead(filePath_, std::ios::binary);
    if ( fileRead.fail() ) throw std::runtime_error("Atom::writeAtomDataToFile_ can not parse file: "+filePath_);
    fileRead.seekg(fileDataPos_, fileRead.beg);
    datablock::tkhdDataBlock tkhdData;
    fileRead.read((char *) &tkhdData, sizeof(tkhdData));
    fileRead.close();

    tkhdData.duration = _byteswap_ulong(movieDuration);
    
    fileWrite.write((char *) &tkhdData, sizeof(tkhdData));
}
/*
void MP4::tkhd::createData(splunkType &splunk)
{
    std::ifstream fileStream(filePath_, std::ios::binary);
    if ( fileStream.fail() ) throw std::runtime_error("tkhd atom can not parse file: "+filePath_);
    datablock::tkhdDataBlock tkhdData;
    fileStream.seekg(fileDataPos_, fileStream.beg);
    fileStream.read((char *) &tkhdData, sizeof(tkhdData));
    tkhdData.duration = _byteswap_ulong(splunk.videoDuration);

    splunk.fileWrite->write((char *) &tkhdData, sizeof(tkhdData));

}
*/
std::string MP4::tkhd::key = "tkhd";

