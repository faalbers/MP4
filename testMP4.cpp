#include "MP4.cpp"
#include <iostream>


int main(int argc, char* argv[])
{
    if ( argc == 1 ) return 1;

    std::cout << "**** CREATE MP4 ****\n\n";

    MP4::MP4 mp4(argv[1]);

    std::cout << "\n**** TEST MP4 ****\n\n";

    //mp4.printHierarchy();
    
    if (false) 
    {    
        for ( auto hdlr : mp4.getTypeAtoms<MP4::hdlr>()) {
            hdlr->printData();
        }
    }
        
    if (false) 
    {    
        for ( auto trak : mp4.getTypeAtoms<MP4::trak>()) {
            trak->printData();
        }
    }

    if (false) 
    {
        mp4.printHierarchy();
    }
    
    if (false)
    {
        mp4.printHierarchyData();  
    }

    if (false)
    {
        for ( auto track : mp4.getTracks())
            track->printHierarchyData();
    }
    
    if (false)
    {
        for ( auto mdia : mp4.getTypeAtoms<MP4::stsz>() )
            mdia->printHierarchyData();
    }

    //mp4.printHierarchyData();

    //mp4.mdatMap();
    //mp4.mdatRemap();

    if (true)
    {
        auto videoTrack = mp4.getTrackFromID(1);
        auto videoSamples = videoTrack->getSamples();
        auto gpmfTrack = mp4.getTrackFromID(4);
        auto gpmfSamples = gpmfTrack->getSamples();
        auto videoDuration = (float) (videoSamples.back().currentTime+videoSamples.back().duration)
            / videoSamples.back().timeScale;
        auto gpmfDuration = (float) (gpmfSamples.back().currentTime+gpmfSamples.back().duration)
            / gpmfSamples.back().timeScale;

        std::cout << videoSamples.size() << std::endl;
        std::cout << gpmfSamples.size() << std::endl;
        std::cout << videoDuration << std::endl;
        std::cout << gpmfDuration << std::endl;
    }

    
    std::cout << "\n**** END ****\n\n";

    return 0;
}