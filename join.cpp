#include "MP4.cpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if ( argc < 4 ) {
        std::cout << "usage: joinGoPro.exe <first MP4 file> <second MP4 file> <joined MP4 file>" << std::endl;
        return 1;
    }

    MP4::MP4 mp4A(argv[1]);
    MP4::MP4 mp4B(argv[2]);

    auto splunk = mp4A.splunkAppend(mp4B);

    splunk.fileWritePath = argv[3];
    mp4A.splunkCreate(splunk);

    return 0;
}