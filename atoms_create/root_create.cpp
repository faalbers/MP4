#include "root_create.hpp"
#include "ftyp_create.hpp"
#include "mdat_create.hpp"
#include "../atoms/root.hpp"
#include <iostream>

MP4::root_create::root_create(splunkType &splunk)
{
    for ( auto atom : splunk.rootAtom->children_ ) {
        if ( atom->key == "ftyp" ) ftyp_create ftyp(splunk);
        else if ( atom->key == "mdat" ) mdat_create mdat(splunk);
        else std::cout << atom->key << " atom not created in root_create\n";
    }
}

