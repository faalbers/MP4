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

    if (true)
    {
        bool showLists = true;
        int count;
        for ( auto track : mp4.getTracks()) {
            std::cout << "\nTrack: " << track->getID() << std::endl;
            std::cout << "\nTime Scale: " << track->getMediaTimeScale() << std::endl;
            std::cout << "Sample descreptions:\n";
            for ( auto sampleD : track->getSampleDescriptions())
                std::cout << "[" << sampleD.ID << "] ( '" << sampleD.dataFormat
                << "', " << sampleD.dataReferenceIndex
                << ", " << sampleD.dataReferenceIndex << " )\n";
            std::cout << "Data References:\n";
            for ( auto dataReference : track->getDataReferences())
                std::cout << "[" << dataReference.ID << "] ( '" << dataReference.reference << "' )\n";
            std::cout << "Sample count: " << track->getSampleCount() << std::endl;
            count = 10;
            if ( showLists )
                for ( auto sample : track->getSamples() ) {
                    std::cout << "[" << sample.ID << "] ( " << sample.duration
                    << ", " << sample.time
                    << ", " << sample.currentTime
                    << ", " << sample.dataSize << " )\n";
                    count--;
                    if ( count == 0 ) {
                        std::cout << "...\n";
                        break;
                    }
                }
            std::cout << "Chunk count: " << track->getChunkCount() << std::endl;
            count = 10;
            if ( showLists )
                for ( auto chunk : track->getChunks() ) {
                    std::cout << "[" << chunk.ID << "] ( " << chunk.samples
                    << ", " << chunk.firstSampleID
                    << ", " << chunk.currentSampleID
                    << ", " << chunk.sampleDescriptionID
                    << ", " << chunk.dataOffset << " )\n";
                    count--;
                    if ( count == 0 ) {
                        std::cout << "...\n";
                        break;
                    }
                }
        }
}

    std::cout << "\n**** END ****\n\n";

    return 0;
}