#ifndef MP4_ATOMCOPYFILE_H
#define MP4_ATOMCOPYFILE_H

#include <string>
#include <fstream>
#include <set>

namespace MP4
{

class atomCopyFile
{
public:
    atomCopyFile(std::string fileName);
    ~atomCopyFile();

    std::string     getFilePath();
    std::ofstream*  getFileWrite();

    void            addExcludeAtom(std::string atomPath);
    void            addFullCopyAtom(std::string atomPath);
    void            addExcludeTrack(uint32_t trackID);

    bool            isTrackExcluded(uint32_t trackID);
    bool            isAtomExcluded(std::string atomPath);

private:
    void            error_(std::string message) const;

    std::string             filePath_;
    std::ofstream           fileStream_;
    std::set<std::string>   excludeAtoms_;
    std::set<std::string>   copyFullAtoms_;
    std::set<uint32_t>      excludeTracks_;

};

}

#endif