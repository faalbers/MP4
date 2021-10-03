#include "MP4.cpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>

int main(int argc, char* argv[])
{
    if ( argc == 1 ) return 1;
    
    auto createStart = std::chrono::high_resolution_clock::now();

    std::cout << "**** CREATE MP4 ****\n\n";

    MP4::MP4 mp4A(argv[1]);

    auto testStart = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(testStart - createStart);
    std::cout << "\nCreate Time: " << ms_int.count() << "ms\n";
    std::cout << "**** TEST MP4 ****\n\n";
    
    //mp4A.printHierarchy();
    mp4A.printHierarchyData();

    if (false) {
        std::map<uint32_t, std::map<uint32_t, uint32_t>> tracksData;
        // testing map data manipulation
        for ( auto track : mp4A.getTracks() ) {
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
        for ( auto track : mp4A.getTracks() ) {
            for ( auto chunk : track->getChunks() ) {
                std::cout << chunk.first << " " << chunk.second.firstSampleID << " " << chunk.second.dataOffset << std::endl;
            }
        }
    }

    if (false) {
        // test getSampleDescriptions
        for ( auto track : mp4A.getTracks() ) {
            for ( auto sDesc : track->getSampleDescriptions() ) {
                std::cout << sDesc.first << " " << sDesc.second.dataFormat << " " << sDesc.second.dataReferenceIndex << std::endl;
            }
        }
    }

    if (false) {
        // test isDataInSameFile
        for ( auto track : mp4A.getTracks() ) {
            if ( track->isDataInSameFile() )
                std::cout << track->getID() << " in same file " << std::endl;
        }
    }


    auto end = std::chrono::high_resolution_clock::now();
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - testStart);
    std::cout << "\nTest Time : " << ms_int.count() << "ms\n";
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - createStart);
    std::cout << "Total Time: " << ms_int.count() << "ms\n";
    std::cout << "**** END ****\n\n";

    return 0;
}