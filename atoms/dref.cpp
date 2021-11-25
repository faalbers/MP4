#include "dref.hpp"
#include "alis.hpp"
#include "url_.hpp"
#include <iostream>

MP4::dref::dref(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock drefData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &drefData, sizeof(drefData));

    dataReferenceCount = XXH_swap32(drefData.numberOfEntries);
    uint32_t ID = 1;

    int64_t childNextPos;
    do {
        parse.setParentPath(path_+"/");
        auto child = makeAtom_(parse);
        childNextPos = child->fileNextPos_;
        fileStream->seekg(childNextPos, fileStream->beg);
        children_.push_back(child);
        ID++;
    } while ( ID <= dataReferenceCount );
}

MP4::dref::dref(std::shared_ptr<atomBuild> build)
    : atom(build)
{
    std::shared_ptr<atom> child;

    auto dataReferences = build->getDataReferences();
    dataReferenceCount = (uint32_t) dataReferences.size();

    for ( auto dataReference : dataReferences ) {
        build->setParentPath(parentPath_ + getKey() + "/");
        if ( dataReference.second.type == "alis" )
            child = std::make_shared<alis>(build);
        else if ( dataReference.second.type == "url " )
            child = std::make_shared<url_>(build);
        else
            error_("dref: constructor can not add reference type: "+dataReference.second.type);
        children_.push_back(child);
    }
}

void MP4::dref::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Data Reference Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "number of entries: " << dataReferenceCount << std::endl;
}

std::string MP4::dref::getKey() const
{
    return key;
}

bool MP4::dref::isDataInSameFile(uint16_t referenceIndex) const
{
    return getDataReferences()[referenceIndex].dataInSameFile;
}

std::map<uint16_t, MP4::dataReferenceEntryType> MP4::dref::getDataReferences() const
{
    std::map<uint16_t, dataReferenceEntryType> dataReferences;
    uint16_t referenceIndex = 1;
    dataReferenceEntryType drefEntry;
    for ( auto referenceAtom : children_ ) {
        drefEntry.type = referenceAtom->getKey();
        if ( drefEntry.type == "alis" )
            drefEntry.dataInSameFile = ((alis *) referenceAtom.get())->dataInSameFile;
        else if ( drefEntry.type == "url " )
            drefEntry.dataInSameFile = ((alis *) referenceAtom.get())->dataInSameFile;
        else
            error_("dref::getDataReferences: can not handle data reference of type: "+drefEntry.type);
        dataReferences[referenceIndex] = drefEntry;
        referenceIndex++;
    }
    return dataReferences;
}

void MP4::dref::write(std::shared_ptr<atomWriteFile> writeFile) const
{
    writeHeader(writeFile);

    writeData(writeFile);
    writeChildren(writeFile);

    writeTail(writeFile);
}

void MP4::dref::writeData(std::shared_ptr<atomWriteFile> writeFile) const
{
    auto fileWrite = writeFile->getFileWrite();
    tableBlock drefData;

    // default settings
    drefData.version.version = 0;
    drefData.version.flag[0] = 0;
    drefData.version.flag[1] = 0;
    drefData.version.flag[2] = 0;
    
    // data settings
    drefData.numberOfEntries = XXH_swap32( dataReferenceCount );

    fileWrite->write((char*) &drefData, sizeof(drefData));
}

const std::string MP4::dref::key = "dref";

