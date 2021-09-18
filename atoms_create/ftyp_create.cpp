#include "ftyp_create.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

MP4::ftyp_create::ftyp_create(splunkType &splunk)
    : atom_create("ftyp", splunk.fileWrite)
{
    createHeader_(true);
    
    // major brand
    splunk.fileWrite->write(std::string("mp41").c_str(), 4);
    // version
    uint32_t version = 0;
    version |= 0x10 << 8;
    version |= 0x18;
    version |= 0x20 << 24;
    version |= 0x13 << 16;
    version = _byteswap_ulong(version);
    splunk.fileWrite->write((char *) &version, sizeof(version));
    //compatible brands
    splunk.fileWrite->write(std::string("mp41").c_str(), 4);

    setSize_();
}

