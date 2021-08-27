#include "MP4.cpp"
#include <iostream>


int main(int argc, char* argv[])
{
    if ( argc == 1 ) return 1;

    std::cout << "**** CREATE MP4 ****\n\n";

    MP4::MP4 mp4(argv[1]);

    std::cout << "\n**** TEST MP4 ****\n\n";

    //mp4.printHierarchy();
    
    for ( auto hdlr : mp4.getTypeAtoms<MP4::hdlr>()) {
        hdlr->printData();
    }
    
    for ( auto trak : mp4.getTypeAtoms<MP4::trak>()) {
        trak->printData();
    }
   
    std::cout << "\n**** END ****\n\n";

    return 0;
}