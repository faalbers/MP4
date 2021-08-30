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
        for ( auto track : mp4.getTracks()) {
            std::cout << "Track: " << track->getID() << std::endl;
            for ( auto format : track->getSampleDataFormats() )
                std::cout << format << std::endl;
        }
    }

    if (true)
    {
        std::string dataFormat = "gpmd";
        float seconds = 5;
        for ( auto track : mp4.getTracks()) {
            //if ( track->hasSampleDataFormat("gpmd") ) {
            if ( track->hasSampleDataFormat(dataFormat) ) {
                std::cout << "gpmd track: " << track->getID() << std::endl;
                if ( track->getSampleDataReference(dataFormat) == "" ) {
                    std::cout << "gpmd data is embedded in same file\n";
                    auto sampleAt = track->getSampleAtTime(seconds);
                    std::cout << "sample at " << seconds << " sec:\n";
                    std::cout << "[" << sampleAt.ID << "] ( " << sampleAt.duration
                    << ", " << sampleAt.time
                    << ", " << sampleAt.currentTime
                    << ", " << sampleAt.dataSize << " )\n";
                    auto sampleChunk = track->sampleToChunk(sampleAt);
                    std::cout << "chunk for that sample:\n";
                    std::cout << "[" << sampleChunk.ID << "] ( " << sampleChunk.samples
                    << ", " << sampleChunk.firstSampleID
                    << ", " << sampleChunk.currentSampleID
                    << ", " << sampleChunk.sampleDescriptionID
                    << ", " << sampleChunk.dataOffset << " )\n";
                    /*
                    for ( auto sample : track->getSamples() )
                        std::cout << "[" << sample.ID << "] ( " << sample.duration
                        << ", " << sample.time
                        << ", " << sample.currentTime
                        << ", " << sample.dataSize << " )\n";
                    */
                    std::cout << "chunks in track: " << track->getChunkCount() << std::endl;
                    /*
                    for ( auto offset : track->getChunkOffsets() )
                        std::cout << offset << std::endl;
                    */
                    /*
                    for ( auto chunk : track->getChunks() ) {
                        std::cout << "[" << chunk.ID << "] ( " << chunk.samples
                        << ", " << chunk.firstSampleID
                        << ", " << chunk.currentSampleID
                        << ", " << chunk.sampleDescriptionID
                        << ", " << chunk.dataOffset << " )\n";
                    }
                    */
                } else {
                    std::cout << "gpmd data is in different file, not getting it ...\n";
                }
            }
        }
    }

    std::cout << "\n**** END ****\n\n";

    return 0;
}