#include "MP4.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>

int main(int argc, char* argv[])
{
    if ( argc == 1 ) return 1;
    
    auto createStart = std::chrono::high_resolution_clock::now();

    std::cout << "**** PARSE MP4 ****\n\n";

    MP4::Parser parserA(argv[1]);

    auto testStart = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(testStart - createStart);
    std::cout << "\nCreate Time: " << ms_int.count() << "ms\n";
    std::cout << "**** TEST MP4 ****\n\n";
    
    //parserA.printHierarchy();
    //parserA.printHierarchyData();

    // create a processor
    MP4::Processor processor;

    if (false) {
        for ( auto dataFormat : parserA.getDataFormats() ) {
            std::cout << dataFormat << std::endl;
        }
    }

    if (false) {
        std::cout << "all tracks:\n";
        for ( auto trackID : parserA.getTrackIDs() ) {
            std::cout << trackID << std::endl;
        }
        std::cout << "'gpmd' dataFormat tracks:\n";
        for ( auto trackID : parserA.getTrackIDs("gpmd") ) {
            std::cout << trackID << std::endl;
        }
    }

/*
    if (false) {
        std::map<uint32_t, std::map<uint32_t, uint32_t>> tracksData;
        // testing map data manipulation
        for ( auto track : parserA.getTracks() ) {
            auto trackID = track->getID();
            for ( auto stsz : track->getTypeAtoms<MP4::stsz>() ) {
                tracksData[trackID] = stsz->stszTable;
            }
        }
        
        for ( auto trackData : tracksData ) {
            std::vector<std::pair<uint32_t, uint32_t>> samples;
            for ( auto sample : trackData.second ) samples.push_back(sample);
            std::reverse(samples.begin(),samples.end());
            std::cout << samples.back().first << std::endl;
        }
    }

    if (false) {
        // test getChunks
        for ( auto track : parserA.getTracks() ) {
            for ( auto chunk : track->getChunks() ) {
                std::cout << chunk.first << " " << chunk.second.firstSampleID << " " << chunk.second.dataOffset << std::endl;
            }
        }
    }

    if (false) {
        // test getSampleDescriptions
        for ( auto track : parserA.getTracks() ) {
            for ( auto sDesc : track->getSampleDescriptions() ) {
                std::cout << sDesc.first << " " << sDesc.second.dataFormat << " " << sDesc.second.dataReferenceIndex << std::endl;
            }
        }
    }

    if (false) {
        // test isDataInSameFile
        for ( auto track : parserA.getTracks() ) {
            if ( track->isDataInSameFile() )
                std::cout << track->getID() << " in same file " << std::endl;
        }
    }

    if (false) {
        // test getSamples
        for ( auto track : parserA.getTracks() ) {
            auto trackData = track->getTrackData();
            std::cout << trackData.dataFormat << " " << trackData.filePath << std::endl;
            std::cout << trackData.trackID << std::endl;
            std::cout << trackData.mediaTimeScale
            << " " << trackData.mediaDuration
            << " " << trackData.samplesDuration << std::endl;
            for ( auto sample : trackData.samples ) {
                std::cout << "[" << sample.first << "] "
                << sample.second.sync
                << " " << sample.second.duration
                << " " << sample.second.time
                << " " << sample.second.filePos
                << " " << sample.second.size << std::endl;
            }
        }
    }
*/
    auto end = std::chrono::high_resolution_clock::now();
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - testStart);
    std::cout << "\nTest Time : " << ms_int.count() << "ms\n";
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - createStart);
    std::cout << "Total Time: " << ms_int.count() << "ms\n";
    std::cout << "**** END ****\n\n";

    return 0;
}