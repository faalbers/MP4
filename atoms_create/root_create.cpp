#include "root_create.hpp"
#include "ftyp_create.hpp"

MP4::root_create::root_create(splunkType &splunk)
{
    ftyp_create ftyp(splunk);
}

