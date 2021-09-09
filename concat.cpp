#include "MP4.cpp"
#include <iostream>
#include <fstream>


int main(int argc, char* argv[])
{
    if ( argc < 3 ) return 1;

    std::cout << "**** CREATE MP4's ****\n\n";

    MP4::MP4 mp4A(argv[1]);
    MP4::MP4 mp4B(argv[1]);

    std::cout << "\n**** TEST MP4 ****\n\n";

    //mp4A.printHierarchy();
    //mp4A.printHierarchyData();

    if (true) {
        MP4::writeSettingsType settings;
        //settings.excludeTrackIDs.insert(1);
        //settings.excludeTrackIDs.insert(2);
        //settings.excludeTrackIDs.insert(3);

        mp4A.writeFile("out.mp4", settings);

        MP4::MP4 mp4New("out.mp4");

        mp4New.printHierarchyData();
    }

    std::cout << "\n**** END ****\n\n";

    return 0;
}