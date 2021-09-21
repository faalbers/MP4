#include "MP4.cpp"
#include <iostream>
#include <fstream>


int main(int argc, char* argv[])
{
    if ( argc < 3 ) return 1;

    std::cout << "**** CREATE MP4's ****\n\n";

    MP4::MP4 mp4A(argv[1]);
    MP4::MP4 mp4B(argv[2]);

    std::cout << "\n**** TEST MP4 ****\n\n";

    //mp4A.printHierarchy();
    //mp4A.printHierarchyData();

    if (false) {
        MP4::writeSettingsType settings;
        //settings.excludeTrackIDs.insert(1);
        settings.excludeTrackIDs.insert(2);
        //settings.excludeTrackIDs.insert(3);
        //settings.excludeTrackIDs.insert(5);

        mp4A.write("out.mp4", settings);

        MP4::MP4 mp4New("out.mp4");

        mp4New.printHierarchyData();
    }

    if (false) {
        MP4::writeSettingsType settings;
        settings.excludeTrackIDs.insert(2);
        settings.excludeTrackIDs.insert(3);
        settings.excludeTrackIDs.insert(4);
        settings.excludeTrackIDs.insert(5);
        
        mp4A.append(mp4B, "outappend.mp4", settings);

        MP4::MP4 mp4New("outappend.mp4");

        mp4New.printHierarchyData();
    }

    if (true)
    {
        //auto splunk = mp4A.splunkGet();
        auto splunk = mp4A.splunkAppend(mp4B);

        splunk.fileWritePath = "..\\testvideos\\justAppendB.MP4";
        mp4A.splunkCreate(splunk);

        MP4::MP4 mp4Append(splunk.fileWritePath);

        //mp4Append.printHierarchy();
        mp4Append.printHierarchyData();
        //mp4A.printHierarchyData();
    }

    std::cout << "\n**** END ****\n\n";

    return 0;
}