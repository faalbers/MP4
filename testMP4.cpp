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
    
        if (false)
    {
        auto splunk = mp4.getSplunk();
        std::map<uint32_t, uint32_t> trackSizeMap;
        for ( auto sample : splunk.samples ) {
            if ( trackSizeMap.count(sample.trackID) == 0)
                trackSizeMap[sample.trackID] = sample.size;
            else
                trackSizeMap[sample.trackID] += sample.size;
        }
        for ( auto trackSize : trackSizeMap )
            std::cout << trackSize.first << " " << trackSize.second << std::endl;
        for ( auto track : mp4.getTracks() ) {
            /*
            std::cout << "Track: " << track->getID() << std::endl;
            uint32_t stszTotal = 0;
            for ( auto stsz : track->getTypeAtoms<MP4::stsz>() ) {
                int index = 1;
                for ( auto size : stsz->stszTable ) {
                    //std::cout << track->getID() << " " << index << " " << size << std::endl;
                    stszTotal += size;
                    index++;
                }
            }
            auto samples = track->getSamples();
            uint32_t getSamplesTotal = 0;
            for ( auto sample : samples.samples ) {
                //std::cout << sample.trackID << " " << sample.ID << " " << sample.size << std::endl;
                getSamplesTotal += sample.size;
            }
            uint32_t splunkTotal = 0;
            for ( auto sample : splunk.samples ) {
                if ( sample.trackID == track->getID() ) {
                    //std::cout << sample.trackID << " " << sample.ID << " " << sample.size << std::endl;
                    splunkTotal += sample.size;
                }
            }
            */
            //std::cout << stszTotal << std::endl;
            //std::cout << getSamplesTotal << std::endl;
            //std::cout << splunkTotal << std::endl;
        }
    }

    if (true)
    {
        auto splunk = mp4.getSplunk();
        /*
        for ( auto sample : splunk.samples ) {
            std::cout
                << "[" << sample.trackID << "]"
                << "[" << sample.ID << "] "
                << sample.time
                << " " << sample.duration 
                << " " << sample.filePos 
                << " " << sample.size
                << " " << sample.filePath << std::endl;
        }
        */
        splunk.filePath = "splunk_out.MP4";
        mp4.createFromSplunk(splunk);

        MP4::MP4 mp4Out(splunk.filePath);

        mp4Out.printHierarchy();
        mp4Out.printHierarchyData();
    }

    if ( false ) {
        for ( auto track : mp4.getTracks() ) {
            uint32_t trackSize = 0;
            for ( auto stsz : track->getTypeAtoms<MP4::stsz>() ) {
                for ( auto size : stsz->stszTable )
                    trackSize += size;
            }
            std::cout << track->getID() << " " << trackSize << std::endl;
        }
    }


    std::cout << "\n**** END ****\n\n";

    return 0;
}