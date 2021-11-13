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
    std::cout << "\nParse Time: " << ms_int.count() << "ms\n";
    std::cout << "**** TEST MP4 ****\n\n";

    //parserA.printHierarchy();
    parserA.printHierarchyData();
    //parserA.printMdatMap();

    if (false) {
        std::cout << "dataFormats:\n" << std::endl;
        for ( auto entry : parserA.getDataFormats() ) {
            std::cout << entry << std::endl;
        }
        std::cout << "componentTypes:\n" << std::endl;
        for ( auto entry : parserA.getComponentTypes() ) {
            std::cout << entry << std::endl;
        }
        std::cout << "componentSubTypes:\n" << std::endl;
        for ( auto entry : parserA.getComponentSubTypes() ) {
            std::cout << entry << std::endl;
        }
    }

    if (false) {
        std::cout << "dataFormat 'gpmd' tracks;\n";
        for ( auto trackID : parserA.getDataFormatTrackIDs("gpmd") ) {
            std::cout << trackID << std::endl;
        }
        std::cout << "componentSubType 'vide' tracks;\n";
        for ( auto trackID : parserA.getComponentSubTypeTrackIDs("vide") ) {
            std::cout << trackID << std::endl;
        }
        std::cout << "componentSubType 'soun' tracks;\n";
        for ( auto trackID : parserA.getComponentSubTypeTrackIDs("soun") ) {
            std::cout << trackID << std::endl;
        }
    }

    if (true) {
        MP4::Processor processor;
        processor.addTrack(parserA,1,1);
        processor.addTrack(parserA,2,2);
        //processor.addTrack(parserA,4,3);

        processor.addUserData(parserA);

        MP4::Writer writer(processor);

        auto filePath = writer.write("writeResult.mp4");

        MP4::Parser parserOut(filePath);
        parserOut.printHierarchy();
        parserOut.printHierarchyData();
        
    }

    if (false) {
        MP4::Writer writer(parserA);

        auto filePath = writer.copyTest("copyResult.mp4");

        std::cout << filePath << std::endl;

        MP4::Parser parserOut(filePath);
        parserOut.printHierarchy();
        parserOut.printHierarchyData();
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