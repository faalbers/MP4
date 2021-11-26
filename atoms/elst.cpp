#include "elst.hpp"
#include "moov.hpp"
#include "mvhd.hpp"
#include <iostream>

MP4::elst::elst(atomParse& parse)
    : atom(parse)
{
    // handle data 
    auto fileStream = parse.getFileStream();

    tableBlock elstData;
    fileStream->seekg(fileDataPos_, fileStream->beg);
    fileStream->read((char*) &elstData, sizeof(elstData));
    elstData.numberOfEntries = XXH_swap32(elstData.numberOfEntries);
    entryDataBlock elstEntryBlock;
    uint32_t entryID = 1;
    do {
        editListEntryType elstEntry;
        fileStream->read((char*) &elstEntryBlock, sizeof(elstEntryBlock));
        elstEntry.trackDuration = XXH_swap32(elstEntryBlock.trackDuration);
        elstEntry.mediaStartTime = XXH_swap32(elstEntryBlock.mediaStartTime);
        elstEntryBlock.mediaRate = XXH_swap32(elstEntryBlock.mediaRate);
        elstEntry.mediaRate = (float)elstEntryBlock.mediaRate / (float)(1 << 16);
        elstTable[entryID] = elstEntry;
        entryID++;
    } while ( entryID <= elstData.numberOfEntries );
}

MP4::elst::elst(std::shared_ptr<atomBuild> build)
    : atom(build)
    , elstTable(build->getEditList())
{
}

void MP4::elst::printData(bool fullLists) const
{
    auto levelCount = std::count(path_.begin(), path_.end(), '/');
    std::string dataIndent = std::string((levelCount-1)*5+1, ' ');
    std::cout << path_ << " (Edit List Atom) ["<< headerSize_ << "]" << std::endl;
    std::cout << dataIndent << "[#] (trackDuration , mediaStartTime, mediaRate)\n";
    int index = 1;
    uint32_t movieTimeScale;
    for ( auto mvhd : moovAtom_->getTypeAtoms<mvhd>() ) movieTimeScale = mvhd->timeScale;
    for ( auto entry : elstTable ) {
        std::cout << dataIndent << "[" << entry.first << "]"
            << " ( " << entry.second.trackDuration
            << " (" << getTimeString(entry.second.trackDuration,  movieTimeScale)
            << "), " << entry.second.mediaStartTime
            << " (" << getTimeString(entry.second.mediaStartTime,  movieTimeScale)
            << "), " << entry.second.mediaRate
            << " )" << std::endl;
        index++;
    }
}

std::string MP4::elst::getKey() const
{
    return key;
}

const std::string MP4::elst::key = "elst";

