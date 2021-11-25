#include "url_.hpp"
#include <iostream>

MP4::url_::url_(atomParse& parse)
    : atom(parse)
    , dataInSameFile(false)
{
    auto fileStream = parse.getFileStream();

    versionBlock drefType;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &drefType, sizeof(drefType));
    if ( drefType.flag[2] == 1) dataInSameFile = true;
    if ( !dataInSameFile )
        throw std::runtime_error("url_ data reference should be in same file");
}

MP4::url_::url_(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    for ( auto dataReference : build->getDataReferences() ) {
        if ( dataReference.second.type == "url " ) { 
            dataInSameFile = dataReference.second.dataInSameFile;
            return;
        }
    }
    error_("alis: constructor did not found data reference of type: 'url '");
}

void MP4::url_::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference type 'url ' Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "dataInSameFile: " << dataInSameFile << std::endl;
}

std::string MP4::url_::getKey() const
{
    return key;
}

void MP4::url_::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    versionBlock drefType;

    // default settings
    drefType.version = 0;
    drefType.flag[0] = 0;
    drefType.flag[1] = 0;
    if ( dataInSameFile ) drefType.flag[2] = 1;
    else drefType.flag[2] = 0;
    
    fileWrite->write((char*) &drefType, sizeof(drefType));
}

const std::string MP4::url_::key = "url ";

