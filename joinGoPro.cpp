#include "MP4.cpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if ( argc < 4 ) {
        std::cout << "usage: joinGoPro.exe <first MP4 file> ... <last MP4 file> <resulting joined MP4 file>" << std::endl;
        return 1;
    }

    MP4::MP4 mp4(argv[1]);
    auto splunk = mp4.splunkGet();

    for ( int i = 2; i < (argc - 1); i++) {
        MP4::MP4 mp4Append(argv[i]);
        MP4::MP4::splunkAppend(splunk, mp4Append);
    }

    splunk.fileWritePath = argv[argc-1];
    mp4.splunkCreate(splunk);

    return 0;
}