#ifndef MP4_FTYP_H
#define MP4_FTYP_H
#include <string>
#include <memory>
#include <vector>
#include "atom.hpp"

/*
FILE TYPE ATOM:
The file type compatibility atom, also called the file type atom, allows the reader to determine whether
this is a type of file that the reader understands.
Specifically, the file type atom identifies the file type specifications with which the file is compatible.
This allows the reader to distinguish among closely related file types, such as QuickTime movie files,
MPEG-4, and JPEG-2000 files (all of which may contain file type atoms, movie atoms, and movie data atoms).
When a file is compatible with more than one specification, the file type atom lists all the compatible types
and indicates the preferred brand, or best use, among the compatible types.
For example, a music player using a QuickTime-compatible file format might identify a file’s best use as
a music file for that player but also identify it as a QuickTime movie.
The file type atom serves a further purpose of distinguishing among different versions or specifications
of the same file type, allowing it to convey more information than the file extension or MIME type alone.
The file type atom also has the advantage of being internal to the file, where it is less subject to
accidental alteration than a file extension or MIME type.
Note: The file type atom described here is functionally identical to the file type box defined in the ISO
specifications for MPEG-4 and JPEG-2000.
The file type atom is optional, but strongly recommended. If present, it must be the first significant atom
in the file, preceding the movie atom (and any free space atoms, preview atom, or movie data atoms).
*/

namespace MP4
{

class ftyp : public atom
{
public:
    ftyp(atomParse &parse);
    ftyp(std::shared_ptr<atomBuild> build);

    void printData(bool fullLists = false);
    void printHierarchyData(bool fullLists = false);

    std::string getKey();

    void writeHeader(std::ofstream &fileWrite);
    void writeData(std::ofstream &fileWrite);

    static std::string          key;
    std::string                 majorBrand;
    uint32_t                    version;
    std::string                 versionString;
    std::vector<std::string>    compatibleBrands;

private:
    typedef struct dataBlock
    {
        char        majorBrand[4];
        uint32_t    version;
    } dataBlock;

};

}
#endif