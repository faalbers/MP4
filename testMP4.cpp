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
        for ( auto track : mp4.getTracks()) {
            if ( track->hasSampleDataFormat("gpmd") ) {
                std::cout << "GPMF track: " << track->getID() << std::endl;
                track->printHierarchyData();
            }
        }
    }

    std::cout << "\n**** END ****\n\n";

    return 0;
}