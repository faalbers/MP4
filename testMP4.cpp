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
    
    if (true)
    {
        mp4.printHierarchyData();  
    }
   
    if (false)
    {
        for ( auto track : mp4.getTracks()) {
            std::cout << "Track: " << track->getID() << std::endl;
            for ( auto format : track->getSampleDataFormats() )
                std::cout << format << std::endl;
        }
    }

    if (true)
    {
        std::string dataFormat = "gpmd";
        float seconds = 20;
        for ( auto track : mp4.getTracks()) {
            //if ( track->hasSampleDataFormat("gpmd") ) {
            if ( track->hasSampleDataFormat(dataFormat) ) {
                std::cout << "gpmd track: " << track->getID() << std::endl;
                if ( track->getSampleDataReference(dataFormat) == "" ) {
                    std::cout << "gpmd data is embedded in same file\n";
                    auto sample = track->getSampleAtTime(seconds);
                    std::cout << "sample at " << seconds << " sec:\n";
                    std::cout << "[" << sample.ID << "] ( " << sample.duration
                    << ", " << sample.time
                    << ", " << sample.timeOffset << " )\n";
                    auto chunk = track->sampleToChunk(sample);
                    std::cout << "chunk for that sample: ["<< chunk.ID << "] ( " << chunk.sampleOffset << " )" << std::endl;
                    auto samples = track->getSamples();
                    std::cout << "samples in track: " << samples.size() << std::endl;
                    /*
                    for ( auto sample : samples )
                        std::cout << "[" << sample.ID << "] ( " << sample.duration
                        << ", " << sample.time
                        << ", " << sample.timeOffset << " )\n";
                    */
                    std::cout << "chunks in track: " << samples.size() << std::endl;
                    auto chunks = track->getChunks();
                    for ( auto chunk : chunks ) {
                        std::cout << "[" << chunk.ID << "] ( " << chunk.samples
                        << ", " << chunk.firstSampleID
                        << ", " << chunk.sampleDescriptionID
                        << ", " << chunk.dataOffset << " )\n";
                    }
                } else {
                    std::cout << "gpmd data is in different file, not getting it ...\n";
                }
            }
        }
    }

    std::cout << "\n**** END ****\n\n";

    return 0;
}