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
        for ( auto track : mp4.getTracks())
            track->printHierarchyData();
    }
    
    if (false)
    {
        for ( auto mdia : mp4.getTypeAtoms<MP4::stsz>() )
            mdia->printHierarchyData();
    }

    //mp4.printHierarchy();
    //mp4.printHierarchyData();

    if (false)
    {
        for ( auto track : mp4.getTracks() ) {
            auto samples = track->getSamples();
            std::cout << "Track ID          : " << samples.trackID << std::endl;
            std::cout << "Sample Count      : " << samples.sampleCount << std::endl;
            std::cout << "File Path         : " << samples.filePath << std::endl;
            std::cout << "Data Format       : " << samples.dataFormat << std::endl;
            std::cout << "Media Time Scale  : " << samples.mediaTimeScale << std::endl;
            std::cout << "Media Time Scale  : " << samples.mediaDuration << std::endl;
            std::cout << "Samples Time Scale: " << samples.samplesDuration << std::endl;
            for ( auto sample : samples.samples )
                std::cout
                    << "[" << sample.trackID << "]"
                    << "[" << sample.ID << "] "
                    << sample.time
                    << " " << sample.duration 
                    << " " << sample.filePos 
                    << " " << sample.size << " " << std::endl;
            std::cout << std::endl;
        }
    }
    
    if (true)
    {
        auto mdat = mp4.getMdatCreate();
        for ( auto sample : mdat.samples ) {
            std::cout
                << "[" << sample.trackID << "]"
                << "[" << sample.ID << "] "
                << sample.time
                << " " << sample.duration 
                << " " << sample.filePos 
                << " " << sample.size << " " << std::endl;
        }
    }

    std::cout << "\n**** END ****\n\n";

    return 0;
}